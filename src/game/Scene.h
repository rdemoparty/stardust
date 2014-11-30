#pragma once

#include <vector>
#include <string>

namespace Acidrain {

    using namespace std;

    class GameObject;

    class GameObjectFactory;

    class Scene {
    public:

        void setGameObjectFactory(GameObjectFactory* objectFactory);

        void add(GameObject* object);

        GameObject* createByName(string name);

        void addNewObjectsToScene();

        vector<GameObject*> objects;
        vector<GameObject*> newlyCreatedObjects;

    private:
        GameObjectFactory* objectFactory;
    };

}