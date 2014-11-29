#pragma once

#include <MovementStrategy.h>
#include <glm/vec2.hpp>

namespace Acidrain {

    using namespace glm;

    class MovementStrategySinusoidal : public MovementStrategy {
    public:
        MovementStrategySinusoidal(float speed);

        virtual void update(float elapsedSeconds) override;

    private:
        float movementPhase;
        float speed;
    };
}