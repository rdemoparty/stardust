#include <AudioSystem.h>
#include <easylogging++.h>
#include <FileSystem.h>

namespace Acidrain {

    int AudioGroup::NEXT_ID = 0;
    map<string, AudioGroup*> AudioGroup::groups;

    AudioGroup::AudioGroup(string name, int channels) {
        this->id = NEXT_ID++;
        this->channels = channels;
        this->name = name;
        groups[name] = this;
    }

    int AudioGroup::getId() const {
        return id;
    }

    const string& AudioGroup::getName() const {
        return name;
    }

    int AudioGroup::getChannels() const {
        return channels;
    }

    const AudioGroup* AudioGroup::byName(string name) {
        return groups[name];
    }

    int AudioGroup::numberOfGroups() {
        return (int) groups.size();
    }

    const AudioGroup* AudioGroup::at(int index) {
        int i = 0;
        for (auto& kv : groups)
            if (i++ == index)
                return kv.second;

        return nullptr;
    }

    int AudioGroup::channelsNeeded() {
        int result = 0;
        for (auto& kv : groups)
            result += kv.second->getChannels();
        return result;
    }

    AudioSystem& AudioSystem::getInstance() {
        static AudioSystem instance;
        return instance;
    }

    AudioSystem::~AudioSystem() {
        LOG(INFO) << "Shutting down audio system";
        Mix_HookMusicFinished(nullptr);
        Mix_CloseAudio();
        while (Mix_Init(0))
            Mix_Quit();
        LOG(INFO) << "Audio system shut down";
    }

    static void musicFinishedCallback() {
        LOG(TRACE) << "Static callback for music finish triggered";
        AUDIOSYS.musicFinishCallback();
    }

    void AudioSystem::init(int sampleRate, int bufferSize) {
        LOG(INFO) << "Initializing audio system with samplerate: " << sampleRate << " and buffer size: " << bufferSize;

        char const* lastError = nullptr;

        int result = Mix_Init(MIX_INIT_OGG);
        lastError = Mix_GetError();
        if (result == 0)
            LOG(ERROR) << "Failed to init SDL mixer. Error: " << lastError;

        result = Mix_OpenAudio(sampleRate, MIX_DEFAULT_FORMAT, 2, bufferSize);
        lastError = Mix_GetError();
        if (result == -1)
            LOG(ERROR) << "Failed to open SDL mixer audio. Error: " << lastError;

        new AudioGroup("UI", 2);
        new AudioGroup("EXPLOSIONS", 4);
        new AudioGroup("PLAYER", 6);
        new AudioGroup("PLAYER_WEAPONS", 1);

        Mix_AllocateChannels(AudioGroup::channelsNeeded());
        int allocatedChannels = Mix_AllocateChannels(-1);
        LOG(INFO) << "Allocated channels: " << allocatedChannels;

        // create channel groups
        int channelStart = 0;
        for (int i = 0; i < AudioGroup::numberOfGroups(); i++) {
            const AudioGroup* group = AudioGroup::at(i);
            if (group == nullptr)
                LOG(FATAL) << "Audio group for index " << i << " is null";

            int lastChannel = channelStart + group->getChannels() - 1;
            int taggedChannels = Mix_GroupChannels(channelStart, lastChannel, group->getId());
            LOG(INFO) << "Added " << taggedChannels << " channels to group " << group->getName();

            channelStart = lastChannel + 1;
        }

        Mix_HookMusicFinished(musicFinishedCallback);
    }

    Mix_Music* AudioSystem::loadSong(const char* URI) {
        if (songs.find(URI) == songs.end()) {
            LOG(INFO) << "Loading song \"" << URI << "\"";

            Mix_Music* handle = Mix_LoadMUS(FILESYS.absolutePath(URI).c_str());
            if (handle == nullptr)
                LOG(FATAL) << "Failed to load music file " << URI << ". Reason: " << Mix_GetError();

            songs[URI] = handle;
        }
        return songs.at(URI);
    }

    Mix_Chunk* AudioSystem::loadSound(const char* URI) {
        if (sounds.find(URI) == sounds.end()) {
            LOG(INFO) << "Loading sound \"" << URI << "\"";

            Mix_Chunk* handle = Mix_LoadWAV(FILESYS.absolutePath(URI).c_str());
            if (handle == nullptr)
                LOG(FATAL) << "Failed to load sound file " << URI << ". Reason: " << Mix_GetError();

            sounds[URI] = handle;
        }
        return sounds.at(URI);
    }

    void AudioSystem::playMusic(const char* uri) {
        Mix_Music* song = loadSong(uri);
        if (currentMusic && Mix_PlayingMusic()) {
            // Don't restart if we are already playing the same song
            if (currentMusic == song)
                return;

            nextMusic = song;
            Mix_FadeOutMusic(500);
        } else {
            currentMusic = song;
            Mix_FadeInMusic(currentMusic, 9999, 500);
        }
    }

    void AudioSystem::stopMusic() {
        nextMusic = nullptr;
        if (currentMusic && Mix_PlayingMusic()) {
            Mix_FadeOutMusic(500);
        }
    }

    void AudioSystem::musicFinishCallback() {
        if (nextMusic) {
            currentMusic = nextMusic;
            nextMusic = nullptr;
            Mix_FadeInMusic(currentMusic, 9999, 500);
        }
    }

    void AudioSystem::playSound(const char* URI, const AudioGroup* group) {
        if (group == nullptr) {
            LOG(ERROR) << "Wanting to play sound on null group";
            return;
        }

        Mix_Chunk* sound = loadSound(URI);
        int channelToPlayOn = Mix_GroupAvailable(group->getId());
        LOG(TRACE) << "Mix_GroupAvailable for group id " << group->getId() << " returned " << channelToPlayOn;
        if (channelToPlayOn == -1) {
            channelToPlayOn = Mix_GroupOldest(group->getId());
            LOG(TRACE) << "Found oldest channel for group with id " << group->getId() << " as channel " << channelToPlayOn;
        }

        if (channelToPlayOn == -1) {
            LOG(ERROR) << "Failed to play sound " << URI << " on group " << group->getName() << ". No channel available.";
            return;
        }

        if (Mix_PlayChannel(channelToPlayOn, sound, 0) < 0)
            LOG(ERROR) << "Failed to play sound " << URI << " on group " << group->getName() << ". Error: " << Mix_GetError();
    }

    void AudioSystem::playSound(const char* URI, const char* groupName) {
        playSound(URI, AudioGroup::byName(groupName));
    }

    void AudioSystem::stopSounds(vector<string> groups) {
        for (auto& groupName : groups) {
            const AudioGroup* group = AudioGroup::byName(groupName);
            if (group != nullptr)
                Mix_FadeOutGroup(group->getId(), 100);
        }
    }
}