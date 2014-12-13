#include <Weapon.h>
#include <GameObject.h>

namespace Acidrain {

    Weapon::Weapon(string bulletType, float shotsPerSecond, vec2 mountingPoint) {
        this->bulletType = bulletType;
        this->mountingPoint = mountingPoint;
        this->fireDelay = 1.0f / shotsPerSecond;
    }

    void Weapon::fireOn() {
        isFiring = true;
        accumulator = fireDelay;
    }

    void Weapon::fireOff() {
        isFiring = false;
    }

    bool Weapon::update(float elapsedSeconds) {
        bool shouldAddBullet = false;

        if (isFiring) {
            accumulator += elapsedSeconds;
            if (accumulator > fireDelay) {
                accumulator = 0;
                shouldAddBullet = true;
            }
        }

        return shouldAddBullet;
    }

    const string& Weapon::getBulletType() const {
        return bulletType;
    }

    const vec2& Weapon::getMountingPoint() const {
        return mountingPoint;
    }
}