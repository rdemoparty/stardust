#include <Entity.h>

#pragma once

namespace Acidrain {

    class Enemy {
    public:

        void update(float dt);

        void render();

    private:
        DrawableEntity entity;
    };
}