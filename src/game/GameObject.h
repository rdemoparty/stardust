#pragma once

#include <Entity.h>
#include <SpritePool.h>

namespace Acidrain {

    using namespace std;

    class Animation;

    class MovementStrategy;

    class GameObject : public DrawableEntity {
    public:
        explicit GameObject();

        virtual ~GameObject();

        void setMovementController(shared_ptr<MovementStrategy> const &movementController);

        void update(float dt);

    protected:
        float movementPhase = 0;
        shared_ptr<MovementStrategy> movementController;
        Animation *animation;
    };
}