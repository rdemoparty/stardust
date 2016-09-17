#include <GameStateGameOver.h>
#include <Stardust.h>
#include <GfxSystem.h>
#include <AudioSystem.h>
#include <InputProvider.h>

#include <easylogging++.h>
#include <glm/ext.hpp>

namespace Acidrain {

    GameStateGameOver& GameStateGameOver::instance() {
        static GameStateGameOver instance;
        return instance;
    }

    void GameStateGameOver::onEnter(Stardust*) {
        LOG(INFO) << "Game over!";
    }

    void GameStateGameOver::onExit(Stardust*) {
    }

    void GameStateGameOver::update(Stardust* game, float elapsedSeconds) {
    }


    void GameStateGameOver::render(Stardust* game, float alpha) {
        GFXSYS.setClearColor(vec3(0.1 * 2, 0.1 * 2, 0.12 * 2));
        GFXSYS.clearScreen();

        GFXSYS.show();
    }

    GameStateGameOver::GameStateGameOver() {
    }

} // namespace Acidrain
