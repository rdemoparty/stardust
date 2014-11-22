#include <Enemy.h>

namespace Acidrain {

    void Enemy::update(float dt) {
//        movementStrategy.updateMovement(entity, dt);
        entity.update(dt);
        // move in a specific way
        // have a shooting strategy
    }

    void Enemy::render() {
        entity.draw();
        // display visual representation
    }

}