#include <GameStatePlayLevel.h>
#include <Stardust.h>
#include <GfxSystem.h>
#include <Level.h>
#include <InputProvider.h>
#include <FpsCounter.h>
#include <GameStateMenu.h>
#include <AudioSystem.h>

namespace Acidrain {

    GameStatePlayLevel& GameStatePlayLevel::instance() {
        static GameStatePlayLevel instance;
        return instance;
    }

    void GameStatePlayLevel::onEnter(Stardust* game) {
        game->level->start();
        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));
    }

    void GameStatePlayLevel::onExit(Stardust* game) {
        game->level->song->stop();
    }

    void GameStatePlayLevel::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyDown(SDL_SCANCODE_ESCAPE))
            game->fsm->changeState(&GameStateMenu::instance());

        game->level->update(elapsedSeconds);
        game->fpsCounter->update(elapsedSeconds);
    }

    void GameStatePlayLevel::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();
        game->level->render(alpha);

        game->drawStats();

        game->fpsCounter->addFrame();
        GFXSYS.show();
    }
}