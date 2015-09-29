#include <GameServiceLocator.h>

namespace Acidrain {

    static shared_ptr<GameObjectFactory> objectFactory;
    static shared_ptr<Scene> sceneInstance;
    static shared_ptr<GameSession> gameSessionInstance;
    static shared_ptr<Camera> cameraInstance;

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

    void GameServiceLocator::gameSession(shared_ptr<GameSession> gameSession) {
        gameSessionInstance = gameSession;
    }

    shared_ptr<GameSession> GameServiceLocator::gameSession() {
        return gameSessionInstance;
    }

    void GameServiceLocator::camera(shared_ptr<Camera> camera) {
        cameraInstance = camera;
    }

    shared_ptr<Camera> GameServiceLocator::camera() {
        return cameraInstance;
    }
}