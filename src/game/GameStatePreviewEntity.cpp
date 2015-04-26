#include <GameStatePreviewEntity.h>
#include <Stardust.h>
#include <GfxSystem.h>
#include <Level.h>
#include <FpsCounter.h>
#include <GameObject.h>
#include <Scene.h>
#include <SpriteAnimationRepository.h>
#include <LevelScriptPreviewEntity.h>
#include <InputProvider.h>
#include <AudioSystem.h>

namespace Acidrain {

    GameStatePreviewEntity& GameStatePreviewEntity::instance() {
        static GameStatePreviewEntity instance;
        return instance;
    }

    void GameStatePreviewEntity::onEnter(Stardust* game) {
        GFXSYS.raiseWindow();
        GFXSYS.setClearColor(vec3(0.1f, 0.0f, 0.1f));

        SpriteAnimationRepository::getInstance().initialize("animations.json");

        game->level = make_shared<Level>();
        game->level->levelScript = make_shared<LevelScriptPreviewEntity>(entityName, game->level->scene.get());
    }

    void GameStatePreviewEntity::onExit(Stardust* game) {
        AUDIOSYS.stopMusic();
        AUDIOSYS.stopSounds({"EXPLOSIONS", "PLAYER", "PLAYER_WEAPONS", "UI"});
    }

    void GameStatePreviewEntity::update(Stardust* game, float elapsedSeconds) {
        if (INPUT.isKeyJustReleased(SDL_SCANCODE_ESCAPE)) {
            game->fsm->changeState(nullptr);
        } else {
            game->level->update(elapsedSeconds);
            game->fpsCounter->update(elapsedSeconds);
        }
    }

    void GameStatePreviewEntity::render(Stardust* game, float alpha) {
        GFXSYS.clearScreen();
        game->level->render(alpha);

        game->drawStats();

        game->fpsCounter->addFrame();
        GFXSYS.show();
    }

    void GameStatePreviewEntity::previewEntity(string entityName) {
        this->entityName = entityName;
    }
}