#include "GameState.h"
#include <GLheaders.h>
#include <Shader.h>
#include <Texture.h>
#include <memory>
#include <Sprite.h>
#include <Animation.h>

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
        std::shared_ptr<SpriteSheet> spriteSheet;

        AnimationData animationData;
        std::shared_ptr<Animation> animation;

        vec2 position = vec2(-250, 0);

        void drawSprite(const Sprite& sprite, const vec2& position);
    };

}
