#include <Animation.h>
#include <Enemy.h>
#include <SpriteAnimationRepository.h>

namespace Acidrain {

    Enemy::Enemy() {
        animation = ANIMREPO.newAnimation("enemy2");
        entity.position = vec2(100, 100);
        entity.size = vec2(64, 64);
    }

    Enemy::~Enemy() {
    }

    void Enemy::update(float dt) {
//        movementStrategy.updateMovement(entity, dt);
        entity.update(dt);
        animation->update(dt);
        // move in a specific way
        // have a shooting strategy
    }

    void Enemy::render(SpritePool& spritePool) {
        entity.currentSprite = animation->getSprite();
        entity.addTo(spritePool);
    }

}