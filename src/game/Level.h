#pragma once

#include <memory>

namespace Acidrain {

    using namespace std;

    class GpuProgramConstantBundle;

    class GpuProgram;

    class Camera;

    class Scene;

    class GameObjectFactory;

    class Starfield;

    class Level {
    public:

        Level();

        virtual ~Level();

        void start();

        void update(float elapsedSeconds);

        void render(float frameAlpha);

        bool isFinished() const;

    private:
        shared_ptr<GpuProgramConstantBundle> gpuProgramConstantBundle;

        shared_ptr<GpuProgram> gpuProgram;

        shared_ptr<Camera> camera;

        shared_ptr<Scene> scene;

        shared_ptr<GameObjectFactory> gameObjectFactory;

        shared_ptr<Starfield> starfield;
    };
}