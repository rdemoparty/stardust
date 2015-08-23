#include <GameStatePlayLevel.h>
#include <Stardust.h>
#include <GfxSystem.h>
#include <Level.h>
#include <InputProvider.h>
#include <FpsCounter.h>
#include <GameStateMenu.h>
#include <AudioSystem.h>
#include <GameSession.h>
#include <LevelScript.h>
#include <GameEvent.h>
#include <Scene.h>

namespace Acidrain {

    GameStatePlayLevel &GameStatePlayLevel::instance() {
        static GameStatePlayLevel instance;
        return instance;
    }

    void GameStatePlayLevel::onEnter(Stardust *game) {
        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));
    }

    void GameStatePlayLevel::onExit(Stardust *game) {
    }

    void GameStatePlayLevel::update(Stardust *game, float elapsedSeconds) {
        if (INPUT.isKeyDown(SDL_SCANCODE_ESCAPE)) {
            AUDIOSYS.stopSounds({"PLAYER", "EXPLOSIONS"});
            game->fsm->changeState(&GameStateMenu::instance());
            return;
        }

        GameSession *gameSession = game->gameSession.get();
        Level *level = game->level.get();
        LevelScript *levelScript = level->levelScript.get();

        // TODO: create individual states for both game over and game completed states
        if (gameSession != nullptr) {

            handleGameEvents(game);

            switch (gameSession->getState()) {
                case GameSessionState::GAME_OVER:
                case GameSessionState::GAME_FINISHED:
                    return;

                case GameSessionState::NEW:
                    levelScript->load(gameSession->getLevelUri());
                    level->start();

                    level->addPlayerToScene();
                    level->addPlatformsToScene();

                    AUDIOSYS.playMusic("main.ogg");

                    gameSession->notifySessionStarted();
                    break;

                case GameSessionState::PLAYING:
                    if (!level->playerExists()) {
                        gameSession->notifyPlayerDeath();
                        if (gameSession->getState() != GameSessionState::GAME_OVER) {
                            level->addPlayerToScene();
                            level->addPlatformsToScene();
                        }
                    }

                    if (level->isFinished()) {
                        gameSession->notifyLevelFinish();
                        if (gameSession->getState() != GameSessionState::GAME_FINISHED) {
                            levelScript->load(gameSession->getLevelUri());
                            level->start();
                        }
                    }

                    game->level->update(elapsedSeconds);
                    game->fpsCounter->update(elapsedSeconds);
                    break;
            }
        }
    }

    void GameStatePlayLevel::handleGameEvents(const Stardust *game) const {
        GameEvent event;
        while ((event = game->level->scene->pollEvent()) != GameEvent::NO_EVENT) {
            switch (event) {
                case GameEvent::LEVEL_END:
                    game->level->finish();
                    break;
                default:break;
            };
        }
    }

    void GameStatePlayLevel::render(Stardust *game, float alpha) {
        GFXSYS.clearScreen();

        if (game->gameSession) {
            switch (game->gameSession->getState()) {
                case GameSessionState::NEW:
                    break;
                case GameSessionState::PLAYING:
                    game->level->render(alpha);
                    game->drawStats();
                    break;
                case GameSessionState::GAME_OVER:
                    break;
                case GameSessionState::GAME_FINISHED:
                    break;
            }
        }

        game->fpsCounter->addFrame();
        GFXSYS.show();
    }
}