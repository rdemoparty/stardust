#include <Scene.h>
#include <GameObject.h>
#include <GameObjectFactory.h>
#include <GfxSystem.h>

namespace Acidrain {

    Scene::Scene(GameObjectFactory* objFactory, const vec2& visibleAreaSize)
            : objectFactory(objFactory), visibleArea(visibleAreaSize) {
        spritePool = make_shared<SpritePool>();
    }

    Scene::~Scene() {
    }

    void Scene::add(GameObject* object) {
        object->setScene(this);
        newlyCreatedObjects.push_back(object);
    }

    void Scene::addNewObjectsToScene() {
        objects.insert(objects.end(), newlyCreatedObjects.begin(), newlyCreatedObjects.end());
        newlyCreatedObjects.clear();
    }

    GameObject* Scene::createByName(string name) {
        if (name == "laser") {
            return objectFactory->createLaser();
        } else {
            return nullptr;
        }
    }

    int Scene::countObjects() const {
        return static_cast<int>(objects.size());
    }

    void Scene::removeEntitiesOutOfVisibleArea() {
        auto i = begin(objects);
        while (i != end(objects)) {
            if (isObjectOutOfVisibleArea(*i)) {
                delete *i;
                i = objects.erase(i);
            } else {
                ++i;
            }
        }
    }

    bool Scene::isObjectOutOfVisibleArea(GameObject* object) {
        const float SIZE_FACTOR_TO_ALLOW = 1.0f;
        const float objectHalfWidth = object->size.x / 2.0f;
        const float objectHalfHeight = object->size.y / 2.0f;

        if ((object->position.y + objectHalfHeight) < -(object->size.y * SIZE_FACTOR_TO_ALLOW))
            return true;

        if ((object->position.y - objectHalfHeight) > (object->size.y * SIZE_FACTOR_TO_ALLOW + visibleArea.y))
            return true;

        if ((object->position.x + objectHalfWidth) < -(object->size.x * SIZE_FACTOR_TO_ALLOW))
            return true;

        if ((object->position.x - objectHalfWidth) > (object->size.y * SIZE_FACTOR_TO_ALLOW + visibleArea.x))
            return true;

        return false;
    }


    void Scene::update(float elapsedSeconds) {
        // add the objects created last frame
        addNewObjectsToScene();

        for (auto& gameObject : objects) {
            gameObject->update(elapsedSeconds);
            gameObject->updateAnimation(elapsedSeconds);
        }

        detectCollisions();
        solveCollisions();

        removeEntitiesOutOfVisibleArea();
    }

    void Scene::draw(shared_ptr<GpuProgram> gpuProgram) {
        spritePool->clear();

        for (auto& gameObject : objects)
            gameObject->addTo(*spritePool);

        GFXSYS.setTransparencyMode(TransparencyMode::Special);
        spritePool->draw(gpuProgram);

        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
        for (auto& gameObject : objects)
            gameObject->collisionHull.draw();
    }

    void Scene::confineEntityInVisibleArea(GameObject* object) {
        if (object->position.x < object->size.x / 2.0f)
            object->position.x = object->size.x / 2.0f;

        if (object->position.x > (visibleArea.x - object->size.x / 2.0f))
            object->position.x = visibleArea.x - object->size.x / 2.0f;

        if (object->position.y < object->size.y / 2.0f)
            object->position.y = object->size.y / 2.0f;

        if (object->position.y > (visibleArea.y - object->size.y / 2.0f))
            object->position.y = visibleArea.y - object->size.y / 2.0f;
    }


    void Scene::detectCollisions() {
        collisions.clear();
        for (auto it = objects.begin(); it != objects.end(); it++) {
            for (auto it2 = it; it2 != objects.end(); it2++) {
                if (it == it2)
                    continue;
                else
                    detectCollisionBetweenGameObjects(*it, *it2);
            }
        }
    }

    void Scene::solveCollisions() {
        // TODO: implement collision result handling
    }

    void Scene::detectCollisionBetweenGameObjects(GameObject* a, GameObject* b) {
        if (a->collisionHull.collidesWith(b->collisionHull)) {
            collisions.push_back(CollisionInfo(a, b, vec2(0, 0)));
        }
    }


    int Scene::countCollisions() const {
        return collisions.size();
    }
}