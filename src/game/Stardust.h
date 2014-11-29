#pragma once

#include <EventSystem.h>
#include <memory>

namespace Acidrain {

    using namespace std;

    class GameObject;

    class GameObjectFactory;

    class Shader;

    class Font;

    class InputProvider;

    class SpritePool;

    class FpsCounter;

    class Starfield;

    class Stardust : public EventListener {
    public:
        Stardust();

        ~Stardust();

        void update(float elapsedSeconds);

        void render();

        bool shouldQuit();

    private:

        shared_ptr<GameObjectFactory> gameObjectFactory;
        vector<GameObject *> gameObjects;

        shared_ptr<FpsCounter> fpsCounter;

        shared_ptr<Shader> shader;

        shared_ptr<Font> font;
        shared_ptr<Font> fontSmall;

        shared_ptr<InputProvider> input;

        shared_ptr<Starfield> starfield;

        shared_ptr<SpritePool> spritePool;

        bool quitGame = false;

        void drawStats();

        virtual void onEvent(SDL_Event const &param);
    };
}
