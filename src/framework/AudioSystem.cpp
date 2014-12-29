#include <AudioSystem.h>
#include <SDL_mixer.h>
#include <easylogging++.h>
#include <FileSystem.h>

namespace Acidrain {

    AudioSystem& AudioSystem::getInstance() {
        static AudioSystem instance;
        return instance;
    }

    AudioSystem::~AudioSystem() {
        Mix_CloseAudio();
        Mix_Quit();
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
    }

    shared_ptr<Song> AudioSystem::loadSong(const char* filename) {
        if (songs.find(filename) == songs.end()) {
            LOG(INFO) << "Loading song \"" << filename << "\"";

            Mix_Music* handle = Mix_LoadMUS(FILESYS.absolutePath(filename).c_str());
            if (handle == nullptr)
                LOG(FATAL) << "Failed to load music file " << filename << ". Reason: " << Mix_GetError();

            songs[filename] = make_shared<Song>(handle);
        }
        return songs.at(filename);
    }

    void Song::play(int loops) {
        if (Mix_PlayMusic(handle, loops) == -1)
            LOG(ERROR) << "Failed to play song. Error: " << Mix_GetError();
    }

    Song::~Song() {
        if (handle != nullptr)
            Mix_FreeMusic(handle);
    }

    Sound::~Sound() {
        if (handle != nullptr)
            Mix_FreeChunk(handle);
    }

    void Sound::play(int channel, int loops) {
        if (Mix_PlayChannel(channel, handle, loops) < 0)
            LOG(ERROR) << "Failed to play sound. Error: " << Mix_GetError();
    }

    shared_ptr<Sound> AudioSystem::loadSound(const char* filename) {
        if (sounds.find(filename) == sounds.end()) {
            LOG(INFO) << "Loading sound \"" << filename << "\"";

            Mix_Chunk* handle = Mix_LoadWAV(FILESYS.absolutePath(filename).c_str());
            if (handle == nullptr)
                LOG(FATAL) << "Failed to load sound file " << filename << ". Reason: " << Mix_GetError();

            sounds[filename] = make_shared<Sound>(handle);
        }
        return sounds.at(filename);
    }
}