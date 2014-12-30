#pragma once

#include <EventSystem.h>
#include <memory>
#include <map>
#include <string>
#include <GameState.h>

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

        shared_ptr<GameStateMachine<Stardust>> fsm;
        shared_ptr<Level> level;
        bool quitGame = false;
        shared_ptr<FpsCounter> fpsCounter;

        void drawStats();

    private:

        shared_ptr<Font> titleFont;
        shared_ptr<Font> fontSmall;

        virtual void onEvent(SDL_Event const& param);
    };
}
