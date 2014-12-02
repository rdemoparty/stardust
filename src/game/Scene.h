#pragma once

#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <memory>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GameObject;

    class GameObjectFactory;

    class SpritePool;

    class GpuProgram;

    class Scene {
    public:

        Scene();

        ~Scene();

        void setGameObjectFactory(GameObjectFactory* objectFactory);

        void setVisibleArea(const vec2& size);

        void add(GameObject* object);

        GameObject* createByName(string name);

        void addNewObjectsToScene();

        void removeEntitiesOutOfVisibleArea();

        void update(float elapsedSeconds);

        void draw(shared_ptr<GpuProgram> gpuProgram);

        void confineEntityInVisibleArea(GameObject* object);

        int countObjects() const;

    private:

        bool isObjectOutOfVisibleArea(GameObject* object);

        vector<GameObject*> objects;
        vector<GameObject*> newlyCreatedObjects;
        shared_ptr<SpritePool> spritePool;
        GameObjectFactory* objectFactory;
        vec2 visibleArea;
    };

}