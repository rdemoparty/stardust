#include <GameServiceLocator.h>

namespace Acidrain {

    static shared_ptr<GameObjectFactory> objectFactory;
    static shared_ptr<Scene> sceneInstance;

    void GameServiceLocator::gameObjectFactory(shared_ptr<GameObjectFactory> factory) {
        objectFactory = factory;
    }

    shared_ptr<GameObjectFactory> GameServiceLocator::gameObjectFactory() {
        return objectFactory;
    }

    void GameServiceLocator::scene(shared_ptr<Scene> s) {
        sceneInstance = s;
    }

    shared_ptr<Scene> GameServiceLocator::scene() {
        return sceneInstance;
    }
}