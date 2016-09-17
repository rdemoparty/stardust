#include <GameStateLevelFinished.h>
#include <Stardust.h>

#include <easylogging++.h>
#include <GameStateCutSceneAfterLevel.h>

namespace Acidrain {

    GameStateLevelFinished& GameStateLevelFinished::instance() {
        static GameStateLevelFinished instance;
        return instance;
    }

    void GameStateLevelFinished::onEnter(Stardust*) {
        LOG(INFO) << "Level finished!";
    }

    void GameStateLevelFinished::update(Stardust* game, float elapsedSeconds) {
        game->fsm->changeState(&GameStateCutSceneAfterLevel::instance());
    }


    GameStateLevelFinished::GameStateLevelFinished() {
    }

} // namespace Acidrain
