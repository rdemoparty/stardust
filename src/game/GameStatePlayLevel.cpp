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
#include <Fbo.h>
#include <FileSystem.h>
#include <GpuProgram.h>
#include <GpuProgramConstants.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Acidrain {

    GameStatePlayLevel::GameStatePlayLevel() {
        fbo = make_shared<Fbo>(1024, 768);
        fboTexture = fbo->getTexture();

        gpuProgram = make_shared<GpuProgram>(
                FILESYS.getFileContent("shaders/postprocess.vs.glsl"),
                FILESYS.getFileContent("shaders/postprocess.ps.glsl")
        );

        gpuProgramConstantBundle = make_shared<GpuProgramConstantBundle>();

        gpuProgramConstantBundle->set("orthoMatrix", ortho(0.0f, 1024.0f, 768.0f, 0.0f, 0.0f, 1.0f));
        int textureSamplerIndex = 0;
        gpuProgramConstantBundle->set("diffuseSampler", textureSamplerIndex);
        gpuProgramConstantBundle->set("resolution", vec2(GFXSYS.drawableWidth(), GFXSYS.drawableHeight()));
        gpuProgramConstantBundle->set("offset", vec2(GFXSYS.getOffsetX(), GFXSYS.getOffsetY()));
        gpuProgramConstantBundle->set("time", totalElapsedTime);
        gpuProgram->addConstants(gpuProgramConstantBundle.get());

    }

    GameStatePlayLevel& GameStatePlayLevel::instance() {
        static GameStatePlayLevel instance;
        return instance;
    }

    void GameStatePlayLevel::onEnter(Stardust* game) {
        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));
    }

    void GameStatePlayLevel::onExit(Stardust* game) {
    }

    void GameStatePlayLevel::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyDown(SDL_SCANCODE_ESCAPE)) {
            AUDIOSYS.stopSounds({"PLAYER", "EXPLOSIONS"});
            game->fsm->changeState(&GameStateMenu::instance());
            return;
        }

        if (INPUT.isKeyJustPressed(SDL_SCANCODE_F9)) {
            renderMode++;
            renderMode = renderMode % 5;
            gpuProgramConstantBundle->set("renderMode", renderMode);
        }

        GameSession* gameSession = game->gameSession.get();
        Level* level = game->level.get();
        LevelScript* levelScript = level->levelScript.get();

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
        totalElapsedTime += elapsedSeconds;
    }

    void GameStatePlayLevel::handleGameEvents(const Stardust* game) const {
        GameEvent event;
        while ((event = game->level->scene->pollEvent()) != GameEvent::NO_EVENT) {
            switch (event) {
                case GameEvent::LEVEL_END:
                    game->level->finish();
                    break;
                case GameEvent::LEVEL_SCROLL_PAUSE:
                    game->level->pause();
                    break;
                case GameEvent::LEVEL_SCROLL_RESUME:
                    game->level->resume();
                    break;
                default:
                    break;
            };
        }
    }

    static bool useFbo = true;

    void GameStatePlayLevel::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();


        if (useFbo) {
            fbo->use();
        }

        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

        if (useFbo) {
            fbo->unuse();

            GFXSYS.setTransparencyMode(TransparencyMode::Opaque);
            gpuProgramConstantBundle->set("time", totalElapsedTime);

            // draw the post processed one
            GFXSYS.clearScreen();
            gpuProgram->use();
            fboTexture->useForUnit(0);
            GFXSYS.renderFullScreenTexturedQuad();
            gpuProgram->unuse();
        }

        game->fpsCounter->addFrame();
        GFXSYS.show();
    }
}