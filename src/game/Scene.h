#pragma once

#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <memory>
#include "GameObject.h"

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GameObject;

    class GameObjectFactory;

    class SpritePool;

    class GpuProgram;

    struct CollisionInfo {
        GameObject* from;
        GameObject* to;
        vec2 impactPoint;

        CollisionInfo(GameObject* from, GameObject* to, vec2 where)
                : from(from), to(to), impactPoint(where) {
        }
    };

    class Scene {
    public:

        Scene(GameObjectFactory* objectFactory, const vec2& visibleArea);

        ~Scene();

        void add(GameObject* object);

        GameObject* createByName(string name);

        void update(float elapsedSeconds);

        void draw(shared_ptr<GpuProgram> gpuProgram);

        void confineEntityInVisibleArea(GameObject* object);

        int countObjects() const;

    private:

        void addNewObjectsToScene();

        void flagEntitiesOutOfVisibleArea();

        void removeDeadEntities();

        bool isObjectOutOfVisibleArea(GameObject* object);

        void detectCollisions();

        void solveCollisions();

        void detectCollisionBetweenGameObjects(GameObject* a, GameObject* b);

        vector<GameObject*> objects;
        vector<GameObject*> newlyCreatedObjects;
        shared_ptr<SpritePool> spritePool;
        GameObjectFactory* objectFactory;
        vec2 visibleArea;
        vector<CollisionInfo> collisions;
    };

}