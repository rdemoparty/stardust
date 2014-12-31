#include <Weapon.h>
#include <GameObject.h>

namespace Acidrain {

    Weapon::Weapon(float shotsPerSecond, string sound) {
        fireDelay = 1.0f / shotsPerSecond;
        soundWhenFired = sound;
    }

    void Weapon::fireOn() {
        isFiring = true;
        firingAccumulator = fireDelay;
    }

    void Weapon::fireOff() {
        isFiring = false;
    }

    bool Weapon::update(float elapsedSeconds) {
        bool shouldSpewBullets = false;

        if (isFiring) {
            firingAccumulator += elapsedSeconds;
            if (firingAccumulator > fireDelay) {
                firingAccumulator = 0;
                shouldSpewBullets = true;
            }
        }

        return shouldSpewBullets;
    }

    void Weapon::addEmitter(WeaponEmitter emitter) {
        emitters.push_back(emitter);
    }

    const vector<WeaponEmitter>& Weapon::getEmitters() const {
        return emitters;
    }

    const string& Weapon::getFireSound() const {
        return soundWhenFired;
    }
}