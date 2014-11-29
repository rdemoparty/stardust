#include <GameObject.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>
#include "MovementStrategy.h"

namespace Acidrain {

    GameObject::GameObject() {
        animation = ANIMREPO.newAnimation("enemy2");
        animation->start();

        position = vec2(100, 100);
        rotation = 0;
        size = vec2(64, 64);
    }

    GameObject::~GameObject() {
    }

    void GameObject::setMovementController(shared_ptr<MovementStrategy> const &movementController) {
        this->movementController = movementController;
        movementController->setControlledEntity(this);
    }

    void GameObject::update(float dt) {
        if (movementController)
            movementController->update(dt);

        // update xform
        Entity::update(dt);

        animation->update(dt);
        currentSprite = animation->getSprite();
    }
}