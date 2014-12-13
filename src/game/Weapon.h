#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <string>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GameObject;

    class GameObjectFactory;

    class Weapon {
    public:

        Weapon(string bulletType, float shotsPerSecond, vec2 mountingPoint);

        void fireOn();

        void fireOff();

        const string& getBulletType() const;

        const vec2& getMountingPoint() const;

        /**
        * @returns true if a bullet needs to be created
        */
        bool update(float elapsedSeconds);

    private:
        string bulletType;
        vec2 mountingPoint;
        bool isFiring = false;
        float fireDelay = 0.05f;
        float accumulator = 0.0f;
    };
}