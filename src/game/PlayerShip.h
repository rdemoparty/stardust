#pragma once

#include <GameObject.h>
#include <memory>

namespace Acidrain {

    using namespace std;

    class InputProvider;

    class PlayerShip : public GameObject {
    public:

        PlayerShip();

        virtual void update(float elapsedSeconds) override;

    private:
        shared_ptr<InputProvider> input;

        void confinePlayerToVisibleArea();
    };

}