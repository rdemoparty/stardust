#include <MovementStrategyLinear.h>
#include <Entity.h>

namespace Acidrain {

    MovementStrategyLinear::MovementStrategyLinear(vec2 dir, float spd)
            : direction(dir), speed(spd) {
    }

    void MovementStrategyLinear::update(float elapsedSeconds) {
        if (controlledEntity != nullptr)
            controlledEntity->position += direction * speed * elapsedSeconds;
    }
}