#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <string>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GameObject;

    class GameObjectFactory;

    struct BulletInfo {
        std::string gameObjectName;
    };

    class Weapon {
    public:

        void fireOn();

        void fireOff();

        vector<BulletInfo> update(float elapsedSeconds);

    private:
        bool isFiring = false;
        float fireDelay = 0.2f;
        float accumulator = 0.0f;
    };
}