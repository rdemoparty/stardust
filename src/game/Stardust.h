#pragma once

#include <EventSystem.h>
#include <memory>
#include <map>
#include <string>
#include <Weapon.h>
#include <Scene.h>

namespace Acidrain {

    using namespace std;

    class GameObject;

    class GameObjectFactory;

    class GpuProgram;

    class Font;

    class InputProvider;

    class SpritePool;

    class FpsCounter;

    class Starfield;

    class GpuProgramConstantBundle;

    class Stardust : public EventListener {
    public:
        Stardust();

        ~Stardust();

        void update(float elapsedSeconds);

        void render();

        bool shouldQuit();

    private:

        Weapon weapon;
        Scene scene;

        GameObject* player;

        shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;

        shared_ptr<GameObjectFactory> gameObjectFactory;

        shared_ptr<FpsCounter> fpsCounter;

        shared_ptr<GpuProgram> gpuProgram;

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
