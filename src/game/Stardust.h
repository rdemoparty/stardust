#pragma once

#include <EventSystem.h>
#include <memory>
#include <map>
#include <string>

namespace Acidrain {

    using namespace std;

    class Font;

    class FpsCounter;

    class Level;

    class Stardust : public EventListener {
    public:
        Stardust();

        ~Stardust();

        void update(float elapsedSeconds);

        void render(float alpha);

        bool shouldQuit();

    private:
        shared_ptr<Level> level;

        shared_ptr<FpsCounter> fpsCounter;

        shared_ptr<Font> font;
        shared_ptr<Font> fontSmall;

        bool quitGame = false;

        void drawStats();

        virtual void onEvent(SDL_Event const& param);
    };
}
