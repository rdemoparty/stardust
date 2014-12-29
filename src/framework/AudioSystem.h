#pragma once

#include <SDL_mixer.h>
#include <memory>
#include <map>

#define AUDIOSYS Acidrain::AudioSystem::getInstance()

namespace Acidrain {

    using namespace std;

    class Sound {
    public:

        Sound(Mix_Chunk* h) : handle(h) {
        }

        ~Sound();

        void play(int channel, int loops = 0);

    private:
        Mix_Chunk* handle;
    };

    class Song {
    public:

        Song(Mix_Music* h) : handle(h) {
        }

        ~Song();

        void play(int loops = -1);

    private:
        Mix_Music* handle;
    };

    class AudioSystem {
    public:

        static AudioSystem& getInstance();

        ~AudioSystem();

        void init(int sampleRate = 22050, int bufferSize = 1024);

        shared_ptr<Song> loadSong(const char* filename);

        shared_ptr<Sound> loadSound(const char* filename);

    private:
        map<const char*, shared_ptr<Song>> songs;
        map<const char*, shared_ptr<Sound>> sounds;
    };

} // namespace Acidrain