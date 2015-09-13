#include <GameServiceLocator.h>

namespace Acidrain {

    static shared_ptr<GameObjectFactory> objectFactory;

    void GameServiceLocator::gameObjectFactory(shared_ptr<GameObjectFactory> factory) {
        objectFactory = factory;
    }

    shared_ptr<GameObjectFactory> GameServiceLocator::gameObjectFactory() {
        return objectFactory;
    }
}