#pragma once

#include <GameObjectFactory.h>
#include <memory>

namespace Acidrain {

    using namespace std;

    class GameServiceLocator {
    public:
        static void gameObjectFactory(shared_ptr<GameObjectFactory> factory);

        static shared_ptr<GameObjectFactory> gameObjectFactory();
    };

} // end of namespace Acidrain