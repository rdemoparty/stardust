#include <GameStateNextLevel.h>
#include <Stardust.h>
#include <GameSession.h>
#include <GameStateGameFinished.h>
#include <GameStateCutSceneBeforeLevel.h>

namespace Acidrain {

    GameStateNextLevel& GameStateNextLevel::instance() {
        static GameStateNextLevel instance;
        return instance;
    }

    void GameStateNextLevel::update(Stardust* game, float elapsedSeconds) {
        game->gameSession->notifyLevelFinish();

        if (game->gameSession->getState() == GameSessionState::GAME_FINISHED) {
            game->fsm->changeState(&GameStateGameFinished::instance());
        } else {
            game->fsm->changeState(&GameStateCutSceneBeforeLevel::instance());
        }
    }

    GameStateNextLevel::GameStateNextLevel() {
    }

} // namespace Acidrain
