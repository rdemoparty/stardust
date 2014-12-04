#pragma once
#include <GameObject.h>

namespace Acidrain {

    class Explosion : public GameObject {
    public:

        Explosion();

        virtual void update(float elapsedSeconds) override;
    };
}