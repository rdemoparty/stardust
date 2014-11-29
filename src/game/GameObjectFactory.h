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

        GameObject *player(vec2 position);

        GameObject *enemy(vec2 position);

        GameObject *enemy2(vec2 position);

    private:
        shared_ptr<InputProvider> input;
    };

}