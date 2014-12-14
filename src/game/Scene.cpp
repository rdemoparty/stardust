#include <Scene.h>
#include <GameObjectFactory.h>
#include <GfxSystem.h>
#include <iostream>

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
        if (name == "enemy.bullet.hit") {
            int i = 1;
        }
        return objectFactory->createByName(name);
    }

    int Scene::countObjects() const {
        return static_cast<int>(objects.size());
    }

    void Scene::flagEntitiesOutOfVisibleArea() {
        for (auto& object : objects) {
            if (object->state.killIfOutsideOfVisibleArea && isObjectOutOfVisibleArea(object)) {
                object->state.isDead = true;
                object->state.deathReason = EntityDeathReason::OutOfVisibleArea;
            }
        }
    }

    void Scene::removeDeadEntities() {
        auto i = begin(objects);
        while (i != end(objects)) {
            auto object = *i;
            if (object->state.isDead && object->state.isToBeRemovedOnDeath) {
                i = objects.erase(i);
                delete object;
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

    bool Scene::confineEntityInVisibleArea(GameObject* object) {
        bool hadToCorrect = false;

        if (object->position.x < object->size.x / 2.0f) {
            object->position.x = object->size.x / 2.0f;
            hadToCorrect = true;
        }

        if (object->position.x > (visibleArea.x - object->size.x / 2.0f)) {
            object->position.x = visibleArea.x - object->size.x / 2.0f;
            hadToCorrect = true;
        }

        if (object->position.y < object->size.y / 2.0f) {
            object->position.y = object->size.y / 2.0f;
            hadToCorrect = true;
        }

        if (object->position.y > (visibleArea.y - object->size.y / 2.0f)) {
            object->position.y = visibleArea.y - object->size.y / 2.0f;
            hadToCorrect = true;
        }

        return hadToCorrect;
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
//        // avoid player collisions in order to find out bug related to bullet hits
//        if (a->state.type == EntityType::Ship && a->state.side == EntitySide::Friendly)
//            return;
//
//        // avoid player collisions in order to find out bug related to bullet hits
//        if (b->state.type == EntityType::Ship && b->state.side == EntitySide::Friendly)
//            return;

        if (!a->state.isCollidable || !b->state.isCollidable) return;
        if (a->state.side == b->state.side) return;
        if (a->state.isDead || b->state.isDead) return;
        // no bullet-to-bullet collision
        if (a->state.type == EntityType::Bullet && b->state.type == EntityType::Bullet) return;

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

    string typeName(EntityType type) {
        switch (type) {
            case EntityType::Ship: return "Ship";
            case EntityType::Bullet: return "Bullet";
            case EntityType::Explosion: return "Explosion";
            default:
                return "unknown";
        }
    }

    string sideName(EntitySide side) {
        switch (side) {
            case EntitySide::Friendly: return "Friendly";
            case EntitySide::Adverse: return "Adverse";
            case EntitySide::Neutral: return "Neutral";
            default:
                return "unknown";
        }
    }

    void dump(GameObject* object) {
        std::ostream& out = std::cout;
        out << "GameObject: {\n";
        out << "\tid: " << object->getId() << "\n";
        out << "\ttype: " << typeName(object->state.type) << "\n";
        out << "\tside: " << sideName(object->state.side) << "\n";
        out << "\tisDead: " << object->state.isDead << "\n";
        out << "\tremoveOnDeath: " << object->state.isToBeRemovedOnDeath << "\n";
        out << "\tposition: " << object->position.x << ", " << object->position.y << "\n";
        out << "}";
        out << std::endl;
    }

    void Scene::dumpEntites() {
        std::cout << "===== Dumping game entities " << std::endl;
        for (auto& gameObject : objects)
            dump(gameObject);
    }
}