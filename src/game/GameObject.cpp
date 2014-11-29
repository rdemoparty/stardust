#include <GameObject.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>
#include "MovementStrategy.h"

namespace Acidrain {

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