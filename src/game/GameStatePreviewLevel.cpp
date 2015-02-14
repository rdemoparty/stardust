#include <GameStatePreviewLevel.h>
#include <Stardust.h>
#include <GfxSystem.h>
#include <Level.h>
#include <FpsCounter.h>
#include <GameObject.h>
#include <Scene.h>
#include <SpriteAnimationRepository.h>
#include <LevelScript.h>
#include <InputProvider.h>

namespace Acidrain {

    GameStatePreviewLevel& GameStatePreviewLevel::instance() {
        static GameStatePreviewLevel instance;
        return instance;
    }

    void GameStatePreviewLevel::onEnter(Stardust* game) {
        GFXSYS.raiseWindow();
        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));

        SpriteAnimationRepository::getInstance().initialize("animations.json");

        game->level = make_shared<Level>();
        game->level->levelScript = make_shared<LevelScript>(game->level->scene.get());
    }

    void GameStatePreviewLevel::onExit(Stardust* game) {
    }

    void GameStatePreviewLevel::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyJustReleased(SDL_SCANCODE_ESCAPE)) {
            game->fsm->changeState(nullptr);
        } else {
            game->level->update(elapsedSeconds);
            game->fpsCounter->update(elapsedSeconds);
        }
    }

    void GameStatePreviewLevel::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();
        game->level->render(alpha);

        game->drawStats();

        game->fpsCounter->addFrame();
        GFXSYS.show();
    }

    void GameStatePreviewLevel::preview(Stardust* game) {
        game->level->levelScript->load("levels/level1.json");
        game->level->levelScript->reset();
    }
}