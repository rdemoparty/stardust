#include "GameState.h"
#include <GLheaders.h>
#include <Shader.h>
#include <Texture.h>
#include <memory>

#pragma once

namespace Acidrain {
    class Stardust {
    public:
        Stardust();

        ~Stardust();

        void process(double elapsedSeconds);

        bool shouldQuit();

        GameState state = GameState::MAIN_MENU;

        std::shared_ptr<Shader> shader;
        std::shared_ptr<Texture> texture;
    };

}
