#include <Weapon.h>
#include <GameObject.h>
#include <GameObjectFactory.h>
#include <iostream>

namespace Acidrain {

    void Weapon::fireOn() {
        isFiring = true;
        accumulator = fireDelay;
    }

    void Weapon::fireOff() {
        isFiring = false;
    }

    vector<BulletInfo> Weapon::update(float elapsedSeconds) {
        vector<BulletInfo> result;

        if (isFiring) {
            accumulator += elapsedSeconds;
            if (accumulator > fireDelay) {
                accumulator = 0;
                BulletInfo bullet;
                bullet.gameObjectName = "laser";
                result.push_back(bullet);
            }
        }

        return result;
    }

}