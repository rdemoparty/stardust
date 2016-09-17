#include <GameStateHandlePlayerDeath.h>
#include <Stardust.h>
#include <GfxSystem.h>

#include <easylogging++.h>
#include <GameSession.h>
#include <GameStateGameOver.h>
#include <GameStatePlayLevel.h>
#include <Level.h>

namespace Acidrain {

    GameStateHandlePlayerDeath& GameStateHandlePlayerDeath::instance() {
        static GameStateHandlePlayerDeath instance;
        return instance;
    }

    void GameStateHandlePlayerDeath::onEnter(Stardust*) {
        LOG(INFO) << "Player died!";
    }

    void GameStateHandlePlayerDeath::update(Stardust* game, float elapsedSeconds) {
        game->gameSession->notifyPlayerDeath();

        if (game->gameSession->getState() == GameSessionState::GAME_OVER) {
            game->fsm->changeState(&GameStateGameOver::instance());
        } else {
            game->level->addPlayerToScene();
            game->level->addPlatformsToScene();
            game->fsm->changeState(&GameStatePlayLevel::instance());
        }
    }

    GameStateHandlePlayerDeath::GameStateHandlePlayerDeath() {
    }

} // namespace Acidrain

