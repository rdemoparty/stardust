#include <GameStateGameFinished.h>
#include <Stardust.h>
#include <GfxSystem.h>

#include <easylogging++.h>

namespace Acidrain {

    GameStateGameFinished& GameStateGameFinished::instance() {
        static GameStateGameFinished instance;
        return instance;
    }

    void GameStateGameFinished::onEnter(Stardust*) {
        LOG(INFO) << "Game finished!";
    }

    void GameStateGameFinished::onExit(Stardust*) {
    }

    void GameStateGameFinished::update(Stardust* game, float elapsedSeconds) {
    }


    void GameStateGameFinished::render(Stardust* game, float alpha) {
        GFXSYS.setClearColor(vec3(0.1 * 2, 0.1 * 2, 0.12 * 2));
        GFXSYS.clearScreen();

        GFXSYS.show();
    }

    GameStateGameFinished::GameStateGameFinished() {
    }

} // namespace Acidrain
