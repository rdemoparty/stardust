#pragma once

#include <GameObjectFactory.h>
#include <Scene.h>
#include <GameSession.h>
#include <memory>
#include <Camera.h>

namespace Acidrain {

    using namespace std;

    class GameServiceLocator {
    public:
        static void gameObjectFactory(shared_ptr<GameObjectFactory> factory);

        static shared_ptr<GameObjectFactory> gameObjectFactory();

        static void scene(shared_ptr<Scene> scene);

        static shared_ptr<Scene> scene();

        static void gameSession(shared_ptr<GameSession> gameSession);

        static shared_ptr<GameSession> gameSession();

        static void camera(shared_ptr<Camera> camera);

        static shared_ptr<Camera> camera();
    };

} // end of namespace Acidrain