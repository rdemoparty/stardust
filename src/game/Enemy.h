#pragma once
#include <Entity.h>
#include <SpritePool.h>

namespace Acidrain {

    class Animation;

    class Enemy {
    public:
        explicit Enemy();
        virtual ~Enemy();

        void update(float dt);

        void render(SpritePool& spritePool);

    private:
        DrawableEntity entity;
        Animation* animation;
    };
}