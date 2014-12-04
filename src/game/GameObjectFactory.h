#pragma once

#include <memory>
#include <glm/vec2.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class InputProvider;

    class GameObject;

    class GameObjectFactory {
    public:

        GameObjectFactory();

        ~GameObjectFactory();

        GameObject* createPlayer(vec2 position);

        GameObject* createEnemy(vec2 position);

        GameObject* createExplosion();

        GameObject* createLaser();

    private:
        shared_ptr<InputProvider> input;
    };

}