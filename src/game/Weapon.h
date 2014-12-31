#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <string>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GameObject;

    class GameObjectFactory;

    struct WeaponEmitter {
        string bulletType;
        vec2 mountingPoint;
    };

    class Weapon {
    public:

        Weapon(float shotsPerSecond, string soundWhenFired);

        void addEmitter(WeaponEmitter emitter);

        const vector<WeaponEmitter>& getEmitters() const;

        const string& getFireSound() const;

        void fireOn();

        void fireOff();

        /**
        * return <code>true</code> if time to release bullets
        */
        bool update(float elapsedSeconds);

    private:
        vector<WeaponEmitter> emitters;
        string soundWhenFired;

        bool isFiring = false;
        float fireDelay = 0.05f;
        float firingAccumulator = 0.0f;
    };
}