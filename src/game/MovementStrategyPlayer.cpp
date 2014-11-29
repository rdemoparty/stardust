#include "MovementStrategyPlayer.h"
#include <glm/glm.hpp>
#include <framework/Entity.h>

namespace Acidrain {

    MovementStrategyPlayer::MovementStrategyPlayer(shared_ptr<InputProvider> inputProvider, float spd)
            : input(inputProvider), speed(spd) {
    }

    void MovementStrategyPlayer::setSpeed(float speed) {
        this->speed = speed;
    }

    void MovementStrategyPlayer::update(float elapsedSeconds) {
        if (controlledEntity != nullptr)
            controlledEntity->position += velocityFromInput() * speed * elapsedSeconds;
    }

    vec2 MovementStrategyPlayer::velocityFromInput() {
        vec2 velocity = vec2(0);

        if (input->isKeyDown(SDL_SCANCODE_LEFT) || input->isJoystickPressedLeft()) {
            velocity.x = -1;
        } else if (input->isKeyDown(SDL_SCANCODE_RIGHT) || input->isJoystickPressedRight()) {
            velocity.x = 1;
        }

        if (input->isKeyDown(SDL_SCANCODE_UP) || input->isJoystickPressedUp()) {
            velocity.y = -1;
        } else if (input->isKeyDown(SDL_SCANCODE_DOWN) || input->isJoystickPressedDown()) {
            velocity.y = 1;
        }

        return length(velocity) > 0 ?
                normalize(velocity) :
                velocity;
    }
}
