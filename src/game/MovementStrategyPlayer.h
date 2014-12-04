#pragma once

#include "MovementStrategy.h"
#include <InputProvider.h>
#include <memory>
#include <glm/vec2.hpp>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class MovementStrategyPlayer : public MovementStrategy {
    public:
        MovementStrategyPlayer(shared_ptr<InputProvider> inputProvider, float speed);

        void setSpeed(float speed);

        virtual void update(float elapsedSeconds) override;

    private:
        vec2 velocityFromInput();

        shared_ptr<InputProvider> input;
        float speed = 100.0f;
    };
}
