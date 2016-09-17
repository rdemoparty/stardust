#include <GameStateNewGame.h>
#include <Stardust.h>

#include <AudioSystem.h>
#include <GameSession.h>
#include "GameStateCutSceneBeforeLevel.h"

namespace Acidrain {

    GameStateNewGame& GameStateNewGame::instance() {
        static GameStateNewGame instance;
        return instance;
    }

    void GameStateNewGame::update(Stardust* game, float elapsedSeconds) {
        AUDIOSYS.playMusic("main.ogg");
        game->gameSession->notifySessionStarted();
        game->fsm->changeState(&GameStateCutSceneBeforeLevel::instance());
    }

    GameStateNewGame::GameStateNewGame() {
    }

} // namespace Acidrain
