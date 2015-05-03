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

    class GameSession;

    class Stardust : public EventListener {
    public:
        explicit Stardust();

        virtual ~Stardust();

        void update(float elapsedSeconds);

        void render(float alpha);

        bool shouldQuit();

        shared_ptr<GameStateMachine<Stardust>> fsm;
        shared_ptr<GameSession> gameSession;
        shared_ptr<Level> level;
        shared_ptr<FpsCounter> fpsCounter;

        bool quitGame = false;

        void drawStats();

    private:
        shared_ptr<Font> statsFont;

        virtual void onEvent(SDL_Event const& param);
    };
}
