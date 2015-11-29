#pragma once

#include <memory>
#include <map>
#include <vector>
#include <string>
#include <SDL_mixer.h>

#define AUDIOSYS Acidrain::AudioSystem::getInstance()

namespace Acidrain {

    using namespace std;

    class AudioGroup {
    public:
        AudioGroup(string name, int channels);

        int getId() const;

        const string& getName() const;

        int getChannels() const;

        // static helpers

        static const AudioGroup* byName(string name);

        static int numberOfGroups();

        static const AudioGroup* at(int index);

        static int channelsNeeded();

    private:
        int id;
        string name;
        int channels;
        static map<string, AudioGroup*> groups;
        static int NEXT_ID;
    };


    class AudioSystem {
    public:

        static AudioSystem& getInstance();

        ~AudioSystem();

        void init(int sampleRate = 22050, int bufferSize = 1024);

        void playMusic(const char* URI);

        void playSound(const char* URI, const AudioGroup* group, int volume = 128);

        void playSound(const char* URI, const char* groupName, int volume = 128);

        void playSound(const string& URI, const AudioGroup* group, int volume = 128);

        void playSound(const string& URI, const char* groupName, int volume = 128);

        void stopSounds(vector<string> groups);

        void stopMusic();

        /**
        * value between 0 and 128
        */
        void setMusicVolume(int value);

        void musicFinishCallback();

    private:
        Mix_Music* loadSong(const char* URI);

        Mix_Chunk* loadSound(const char* URI);

        Mix_Music* currentMusic;
        Mix_Music* nextMusic;

        map<const char*, Mix_Chunk*> sounds;
        map<const char*, Mix_Music*> songs;
    };

} // namespace Acidrain
