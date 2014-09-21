#include "GameState.h"

#pragma once

namespace Acidrain {
    class Stardust {
    public:
        Stardust();

        ~Stardust();

        void process(double elapsedSeconds);

        bool shouldQuit();

        GameState state = GameState::MAIN_MENU;
    };

}
