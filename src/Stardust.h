#include "GameState.h"
#include "EventSystem.h"
#include "InputManager.h"
#include <GLheaders.h>
#include <Shader.h>
#include <Texture.h>
#include <memory>
#include <Sprite.h>
#include <Animation.h>
#include <Font.h>
#include <Starfield.h>

#pragma once

namespace Acidrain {
    class Stardust : public EventListener {
    public:
        Stardust();

        ~Stardust();

        void process(float elapsedSeconds);

        bool shouldQuit();
        
    private:

        GameState state = GameState::MAIN_MENU;

        std::shared_ptr<Shader> shader;
        std::shared_ptr<SpriteSheet> spriteSheet;

        AnimationData animationData;
        std::shared_ptr<Animation> animation;

        std::shared_ptr<Font> font;
        std::shared_ptr<Font> fontSmall;

        std::shared_ptr<InputManager> input;

        std::shared_ptr<Starfield> starfield;

        vec2 position = vec2(0, 0);

        bool quitGame = false;

        void drawSprite(const Sprite& sprite, const vec2& position);

        virtual void onEvent(SDL_Event const &param);
    };

}
