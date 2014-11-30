#include <Scene.h>
#include <GameObject.h>
#include <GameObjectFactory.h>

namespace Acidrain {

    void Scene::add(GameObject* object) {
        object->setScene(this);
        newlyCreatedObjects.push_back(object);
    }

    void Scene::addNewObjectsToScene() {
        objects.insert(objects.end(), newlyCreatedObjects.begin(), newlyCreatedObjects.end());
        newlyCreatedObjects.clear();
    }

    void Scene::setGameObjectFactory(GameObjectFactory* objectFactory) {
        this->objectFactory = objectFactory;
    }

    GameObject* Scene::createByName(string name) {
        if (name == "laser") {
            return objectFactory->createLaser();
        } else {
            return nullptr;
        }
    }
}