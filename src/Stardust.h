#pragma once

#include "GameState.h"
#include "EventSystem.h"
#include "InputProvider.h"
#include "SpritePool.h"
#include <GLheaders.h>
#include <Shader.h>
#include <Texture.h>
#include <memory>
#include <Sprite.h>
#include <Animation.h>
#include <Font.h>
#include <Starfield.h>
#include <Entity.h>
#include <Randomizer.h>

#include <FpsCounter.h>

namespace Acidrain {

    using namespace glm;

    class Stardust : public EventListener {
    public:
        Stardust();

        ~Stardust();

        void update(float elapsedSeconds);

        void render();

        bool shouldQuit();

    private:

        FpsCounter fpsCounter;

        Randomizer rng;

        GameState state = GameState::MAIN_MENU;

        std::shared_ptr<Shader> shader;
        std::shared_ptr<SpriteSheet> spriteSheet;

        AnimationData animationData;
        std::shared_ptr<Animation> animation;

        std::shared_ptr<Font> font;
        std::shared_ptr<Font> fontSmall;

        std::shared_ptr<InputProvider> input;

        std::shared_ptr<Starfield> starfield;

        DrawableEntity entity;

        vector<shared_ptr<DrawableEntity>> npcs;
        vector<vec2> velocities;

        vec2 position = vec2(0, 0);

        SpritePool spritePool;

        bool quitGame = false;

        void drawSprite(const Sprite& sprite, const vec2& position);

        void drawStats();

        virtual void onEvent(SDL_Event const& param);
    };

}
