#pragma once

#include <vector>
#include <string>
#include <glm/vec2.hpp>
#include <memory>
#include <GameObject.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GameObject;

    class GameObjectFactory;

    class SpritePool;

    class GpuProgram;

    class Camera;

    struct CollisionInfo {
        GameObject* from;
        GameObject* to;
        vec2 impactPoint;

        CollisionInfo(GameObject* from, GameObject* to, vec2 where)
                : from(from), to(to), impactPoint(where) {
        }
    };

    struct CollidableDiscriminator {
        CollidableDiscriminator(EntityType type, EntitySide side) {
            id = side * 10 + type;
        }
        int id;
    };

    class CollisionMatrix {
    public:

        void registerCollidables(const CollidableDiscriminator& from, const CollidableDiscriminator& to) {
            matrix[from.id][to.id] = true;
            matrix[to.id][from.id] = true;
        }

        inline bool areCollidable(const CollidableDiscriminator& from, const CollidableDiscriminator& to) const {
            return matrix[from.id][to.id];
        }

    private:
        const static int SIZE = 40;
        bool matrix[SIZE][SIZE];
    };


    class Scene {
    public:

        Scene(GameObjectFactory* objectFactory, const vec2& visibleArea);

        ~Scene();

        void add(GameObject* object, int zOrder = 0);

        GameObject* createByName(string name);

        GameObject* getById(long id);

        /**
         * Removes all entities from both the active as well as newly created objects list
         */
        void clear();

        void update(float elapsedSeconds);

        void draw(shared_ptr<GpuProgram> gpuProgram, float frameAlpha);

        bool confineEntityInVisibleArea(GameObject* object);

        void dumpEntities();

        int countObjects() const;

        void setCamera(shared_ptr<Camera> camera);

        void shakeCamera(float amount);

        void changeObjectBrain(GameObject* pObject, char const* const string);

    private:

        void addNewObjectsToScene();

        void flagEntitiesOutOfVisibleArea();

        void removeDeadEntities();

        bool isObjectOutOfVisibleArea(GameObject* object);

        void detectCollisions();

        void solveCollisions();

        void detectCollisionBetweenGameObjects(GameObject* a, GameObject* b);

        CollisionMatrix collisionMatrix;
        shared_ptr<Camera> camera;
        vector<GameObject*> objects;
        vector<GameObject*> newlyCreatedObjects;
        shared_ptr<SpritePool> spritePool;
        GameObjectFactory* objectFactory;
        vec2 visibleArea;

        vector<CollisionInfo> collisions;
    };
}