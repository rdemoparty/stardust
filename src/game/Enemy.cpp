#include <Animation.h>
#include <Enemy.h>
#include <SpriteAnimationRepository.h>

namespace Acidrain {

    Enemy::Enemy() {
        animation = ANIMREPO.newAnimation("enemy2");
        animation->start();

        position = vec2(100, 100);
        rotation = 0;
        size = vec2(64, 64);
    }

    Enemy::~Enemy() {
    }

    void Enemy::update(float dt) {
        // move in a specific way
        // have a shooting strategy
        movementPhase += 2 * dt;

//        position.y += 50.0f * dt;
        position.y = cos(movementPhase) * 100.0f + 300;
        position.x = sin(movementPhase) * 100.0f + 500;

        // update xform
        Entity::update(dt);

        animation->update(dt);
        currentSprite = animation->getSprite();
    }
}