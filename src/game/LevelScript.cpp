#include <LevelScript.h>
#include <Scene.h>

namespace Acidrain {

    LevelScript::LevelScript(Scene* s) : scene(s) {
    }

    LevelScript::~LevelScript() {
    }

    void LevelScript::reset() {
        timeUntilNextSpawn = 5;
    }

    void LevelScript::update(float elapsedSeconds) {
        timeUntilNextSpawn -= elapsedSeconds;
        if (timeUntilNextSpawn < 0) {
            GameObject* enemy = scene->createByName("enemy");
            enemy->currentState.position = vec2(rand() % 1024, -64);
            scene->add(enemy);
            timeUntilNextSpawn = rand() % 3 + 1;
        }
    }
}