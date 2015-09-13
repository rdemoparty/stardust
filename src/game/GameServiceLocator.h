#pragma once

#include <GameObjectFactory.h>
#include <Scene.h>
#include <memory>

namespace Acidrain {

    using namespace std;

    class GameServiceLocator {
    public:
        static void gameObjectFactory(shared_ptr<GameObjectFactory> factory);

        static shared_ptr<GameObjectFactory> gameObjectFactory();

        static void scene(shared_ptr<Scene> scene);

        static shared_ptr<Scene> scene();
    };

} // end of namespace Acidrain