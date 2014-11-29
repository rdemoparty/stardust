#pragma once

#include <MovementStrategy.h>
#include <glm/vec2.hpp>

namespace Acidrain {

    using namespace glm;

    class MovementStrategyLinear : public MovementStrategy {
    public:
        MovementStrategyLinear(vec2 direction, float speed);

        virtual void update(float elapsedSeconds) override;

    private:
        vec2 direction;
        float speed;
    };
}