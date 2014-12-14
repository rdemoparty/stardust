#pragma once

#include <EventSystem.h>
#include <memory>
#include <map>
#include <string>

namespace Acidrain {

    using namespace std;

    class GameObject;

    class GameObjectFactory;

    class GpuProgram;

    class Font;

    class FpsCounter;

    class Starfield;

    class GpuProgramConstantBundle;

    class Scene;

    class Stardust : public EventListener {
    public:
        Stardust();

        ~Stardust();

        void update(float elapsedSeconds);

        void render();

        bool shouldQuit();

    private:

        float shakeFactor = 10;

        shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;

        shared_ptr<GpuProgram> gpuProgram;

        shared_ptr<Scene> scene;

        shared_ptr<GameObjectFactory> gameObjectFactory;

        shared_ptr<FpsCounter> fpsCounter;

        shared_ptr<Font> font;
        shared_ptr<Font> fontSmall;

        shared_ptr<Starfield> starfield;

        bool quitGame = false;

        void drawStats();

        virtual void onEvent(SDL_Event const& param);
    };
}
