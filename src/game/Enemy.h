#pragma once
#include <Entity.h>
#include <SpritePool.h>

namespace Acidrain {

    class Animation;

    class Enemy : public DrawableEntity {
    public:
        explicit Enemy();
        virtual ~Enemy();

        void update(float dt);

    private:
        float movementPhase = 0;
        Animation* animation;
    };
}