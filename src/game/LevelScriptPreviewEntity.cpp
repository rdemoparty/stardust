#include <easylogging++.h>
#include <LevelScriptPreviewEntity.h>
#include <Scene.h>

namespace Acidrain {

    LevelScriptPreviewEntity::LevelScriptPreviewEntity(string previewedEntityRecipe, Scene* s) :
            LevelScript(s), entityName(previewedEntityRecipe) {
    }

    LevelScriptPreviewEntity::~LevelScriptPreviewEntity() {
    }

    void LevelScriptPreviewEntity::update(float elapsedSeconds) {
        if (scene->countObjects() == 0) {
            GameObject* enemy = scene->createByName(entityName);
            enemy->currentState.position = vec2(rand() % 1024, -64);
            scene->add(enemy);
        }
    }
}