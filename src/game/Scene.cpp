#include <Scene.h>
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
        object->spawned();
    }

    void Scene::addNewObjectsToScene() {
        objects.insert(objects.end(), newlyCreatedObjects.begin(), newlyCreatedObjects.end());
        newlyCreatedObjects.clear();
    }

    GameObject* Scene::createByName(string name) {
        return objectFactory->createByName(name);
//        if (name == "laser")
//            return objectFactory->createLaser();
//        if (name == "enemyLaser")
//            return objectFactory->createEnemyLaser();
//        else if (name == "explosion")
//            return objectFactory->createExplosion();
//        else
//            return nullptr;
    }

    int Scene::countObjects() const {
        return static_cast<int>(objects.size());
    }

    void Scene::flagEntitiesOutOfVisibleArea() {
        for (auto object : objects) {
            if (object->state.killIfOutsideOfVisibleArea && isObjectOutOfVisibleArea(object)) {
                object->state.isDead = true;
                object->state.deathReason = EntityDeathReason::OutOfVisibleArea;
            }
        }
    }

    void Scene::removeDeadEntities() {
        auto i = begin(objects);
        while (i != end(objects)) {
            if ((*i)->state.shouldRemove()) {
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
        // remove entities killed the previous frame
        removeDeadEntities();

        // add the objects created last frame
        addNewObjectsToScene();

        for (auto& gameObject : objects) {
            gameObject->update(elapsedSeconds);
            gameObject->updateAnimation(elapsedSeconds);
        }

        detectCollisions();
        solveCollisions();
        flagEntitiesOutOfVisibleArea();
    }

    void Scene::draw(shared_ptr<GpuProgram> gpuProgram) {
        GFXSYS.setTransparencyMode(TransparencyMode::Special);
        drawObjectsOfType(EntityType::Ship, gpuProgram);
        drawObjectsOfType(EntityType::Bullet, gpuProgram);

        GFXSYS.setTransparencyMode(TransparencyMode::Additive);
        drawObjectsOfType(EntityType::Explosion, gpuProgram);

//        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
//        for (auto& gameObject : objects)
//            gameObject->collisionHull.draw();
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

        for (auto i = objects.begin(); i != objects.end(); i++)
            for (auto j = i; j != objects.end(); j++)
                if (i != j)
                    detectCollisionBetweenGameObjects(*i, *j);
    }

    void Scene::solveCollisions() {
        for (auto& collisionInfo : collisions) {
            GameObject* a = collisionInfo.from;
            GameObject* b = collisionInfo.to;
            if (!a->state.isDead && !b->state.isDead) {
                a->inflictDamage(b->state.damageProvidedOnCollision);
                b->inflictDamage(a->state.damageProvidedOnCollision);
            }
        }
    }

    void Scene::detectCollisionBetweenGameObjects(GameObject* a, GameObject* b) {
        if (!a->state.isCollidable || !b->state.isCollidable) return;
        if (a->state.side == b->state.side) return;
        if (a->state.isDead || b->state.isDead) return;
        // no bullet-to-bullet collision
        if (a->state.type == EntityType::Bullet && b->state.type == a->state.type) return;

        if (a->collisionHull.collidesWith(b->collisionHull)) {
            collisions.push_back(CollisionInfo(a, b, (a->position + b->position) / 2.0f));
        }
    }

    void Scene::drawObjectsOfType(EntityType type, shared_ptr<GpuProgram> gpuProgram) {
        spritePool->clear();

        for (auto& gameObject : objects)
            if (gameObject->state.type == type)
                gameObject->addTo(*spritePool);

        spritePool->draw(gpuProgram);
    }
}