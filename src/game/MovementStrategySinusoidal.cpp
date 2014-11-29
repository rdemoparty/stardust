#include <MovementStrategySinusoidal.h>

namespace Acidrain {

    MovementStrategySinusoidal::MovementStrategySinusoidal(float spd)
            : speed(spd) {
    }

    void MovementStrategySinusoidal::update(float elapsedSeconds) {
        movementPhase += speed * elapsedSeconds;
        if (controlledEntity != nullptr) {
            controlledEntity->position.y = cos(movementPhase) * 100.0f + 300;
            controlledEntity->position.x = sin(movementPhase) * 100.0f + 500;
        }
    }
}