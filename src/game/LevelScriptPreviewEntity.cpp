#include <easylogging++.h>
#include <LevelScriptPreviewEntity.h>
#include <Scene.h>
#include <InputProvider.h>
#include <GameServiceLocator.h>

namespace Acidrain {

    LevelScriptPreviewEntity::LevelScriptPreviewEntity(string previewedEntityRecipe, Scene* s) :
            LevelScript(s), entityName(previewedEntityRecipe) {
    }

    LevelScriptPreviewEntity::~LevelScriptPreviewEntity() {
    }

    void LevelScriptPreviewEntity::update(float elapsedSeconds) {
        if (scene->countObjects() == 0 || INPUT.isKeyJustPressed(SDL_SCANCODE_RETURN)) {
            GameObject* entity = GameServiceLocator::gameObjectFactory()->createByName(entityName);
            entity->currentState.position = vec2(rand() % 1024, 64);
            entity->previousState.position = entity->currentState.position;
            scene->add(entity);
        }
    }
}