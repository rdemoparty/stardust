#include <Scene.h>
#include <GameObjectFactory.h>
#include <GfxSystem.h>
#include <iostream>
#include <Randomizer.h>
#include <Camera.h>
#include <algorithm>

namespace Acidrain {

    Scene::Scene(GameObjectFactory* objFactory, const vec2& visibleAreaSize)
            : objectFactory(objFactory), visibleArea(visibleAreaSize) {
        spritePool = make_shared<SpritePool>();

        collisionMatrix.registerCollidables(
                CollidableDiscriminator(EntityType::Bullet, EntitySide::Friendly),
                CollidableDiscriminator(EntityType::Ship, EntitySide::Adverse)
        );
        collisionMatrix.registerCollidables(
                CollidableDiscriminator(EntityType::Bullet, EntitySide::Adverse),
                CollidableDiscriminator(EntityType::Ship, EntitySide::Friendly)
        );
        collisionMatrix.registerCollidables(
                CollidableDiscriminator(EntityType::Ship, EntitySide::Adverse),
                CollidableDiscriminator(EntityType::Ship, EntitySide::Friendly)
        );
        collisionMatrix.registerCollidables(
                CollidableDiscriminator(EntityType::Bonus, EntitySide::Friendly),
                CollidableDiscriminator(EntityType::Ship, EntitySide::Friendly)
        );
    }

    Scene::~Scene() {
    }

    void Scene::add(GameObject* object, int zOrder) {
        object->setScene(this);
        object->state.zOrder = zOrder;
        newlyCreatedObjects.push_back(object);
        object->spawned();
    }

    void Scene::addNewObjectsToScene() {
        objects.insert(objects.end(), newlyCreatedObjects.begin(), newlyCreatedObjects.end());
        newlyCreatedObjects.clear();
    }

    GameObject* Scene::createByName(string name) {
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

    void Scene::setCamera(shared_ptr<Camera> camera) {
        this->camera = camera;
    }

    void Scene::shakeCamera(float amount) {
        if (camera)
            camera->shake(amount);
    }

    bool Scene::isObjectOutOfVisibleArea(GameObject* object) {
        const float SIZE_FACTOR_TO_ALLOW = 1.0f;
        const float objectHalfWidth = object->currentState.size.x / 2.0f;
        const float objectHalfHeight = object->currentState.size.y / 2.0f;

        if ((object->currentState.position.y + objectHalfHeight) < -(object->currentState.size.y * SIZE_FACTOR_TO_ALLOW))
            return true;

        if ((object->currentState.position.y - objectHalfHeight) > (object->currentState.size.y * SIZE_FACTOR_TO_ALLOW + visibleArea.y))
            return true;

        if ((object->currentState.position.x + objectHalfWidth) < -(object->currentState.size.x * SIZE_FACTOR_TO_ALLOW))
            return true;

        if ((object->currentState.position.x - objectHalfWidth) > (object->currentState.size.y * SIZE_FACTOR_TO_ALLOW + visibleArea.x))
            return true;

        return false;
    }

    struct {
        bool operator()(GameObject* a, GameObject* b) {
            if (a->state.zOrder < b->state.zOrder)
                return true;
            else if (a->state.zOrder > b->state.zOrder)
                return false;

            // zOrders are equal. we care for type of entity then
            // order is: scenery, ship, bullets, explosions
            if (a->state.type < b->state.type)
                return true;

            return false;
        }
    } DrawComparator;

    void Scene::update(float elapsedSeconds) {
        // remove entities killed the previous frame
        removeDeadEntities();

        // add the objects created last frame
        addNewObjectsToScene();

        for (auto& gameObject : objects) {
            gameObject->updateAnimation(elapsedSeconds);
            gameObject->update(elapsedSeconds);
        }

        detectCollisions();
        solveCollisions();
        flagEntitiesOutOfVisibleArea();

        sort(objects.begin(), objects.end(), DrawComparator);
    }

    TransparencyMode transparencyModeByObjectType(EntityType type) {
        if (type == EntityType::Explosion)
            return TransparencyMode::Additive;
        else
            return TransparencyMode::Special;
    }

    void Scene::draw(shared_ptr<GpuProgram> gpuProgram, float frameAlpha) {
        TransparencyMode currentTransparencyMode = TransparencyMode::Opaque;
        int currentZOrder = -9999999;

        for (auto& gameObject : objects) {

            TransparencyMode transparencyMode = transparencyModeByObjectType(gameObject->state.type);
            bool transparencyModeChanged = transparencyMode != currentTransparencyMode;
            bool zOrderChanged = gameObject->state.zOrder > currentZOrder;

            if (transparencyModeChanged || zOrderChanged) {
                GFXSYS.setTransparencyMode(currentTransparencyMode);
                spritePool->draw(gpuProgram);
                spritePool->clear();
                currentTransparencyMode = transparencyMode;
                currentZOrder = gameObject->state.zOrder;
            }

            gameObject->setRenderStateAt(frameAlpha);
            gameObject->addTo(*spritePool);
        }

        // draw the last batch
        GFXSYS.setTransparencyMode(currentTransparencyMode);
        spritePool->draw(gpuProgram);
        spritePool->clear();

//        GFXSYS.setTransparencyMode(TransparencyMode::Transparent);
//        for (auto& gameObject : objects)
//            gameObject->collisionHull.draw();
    }

    bool Scene::confineEntityInVisibleArea(GameObject* object) {
        bool hadToCorrect = false;

        if (object->currentState.position.x < object->currentState.size.x / 2.0f) {
            object->currentState.position.x = object->currentState.size.x / 2.0f;
            hadToCorrect = true;
        }

        if (object->currentState.position.x > (visibleArea.x - object->currentState.size.x / 2.0f)) {
            object->currentState.position.x = visibleArea.x - object->currentState.size.x / 2.0f;
            hadToCorrect = true;
        }

        if (object->currentState.position.y < object->currentState.size.y / 2.0f) {
            object->currentState.position.y = object->currentState.size.y / 2.0f;
            hadToCorrect = true;
        }

        if (object->currentState.position.y > (visibleArea.y - object->currentState.size.y / 2.0f)) {
            object->currentState.position.y = visibleArea.y - object->currentState.size.y / 2.0f;
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

    void Scene::detectCollisionBetweenGameObjects(GameObject* a, GameObject* b) {
        if (!a->state.isCollidable || !b->state.isCollidable) return;
        if (a->state.isDead || b->state.isDead) return;

        CollidableDiscriminator d1 = CollidableDiscriminator(a->state.type, a->state.side);
        CollidableDiscriminator d2 = CollidableDiscriminator(b->state.type, b->state.side);
        if (!collisionMatrix.areCollidable(d1, d2))
            return;

        if (a->collisionHull.collidesWith(b->collisionHull))
            collisions.push_back(
                    CollisionInfo(
                            a, b,
                            (a->currentState.position + b->currentState.position) / 2.0f));
    }

    void Scene::solveCollisions() {
        for (auto& collisionInfo : collisions) {
            GameObject* a = collisionInfo.from;
            GameObject* b = collisionInfo.to;

            if (!a->state.isDead && !b->state.isDead) {
                a->inflictDamage(b->state.damageProvidedOnCollision, b);
                b->inflictDamage(a->state.damageProvidedOnCollision, a);
            }
        }
    }

    string typeName(EntityType type) {
        switch (type) {
            case EntityType::Ship:
                return "Ship";
            case EntityType::Bullet:
                return "Bullet";
            case EntityType::Explosion:
                return "Explosion";
            case EntityType::Scenery:
                return "Scenery";
            default:
                return "unknown";
        }
    }

    string sideName(EntitySide side) {
        switch (side) {
            case EntitySide::Friendly:
                return "Friendly";
            case EntitySide::Adverse:
                return "Adverse";
            case EntitySide::Neutral:
                return "Neutral";
            default:
                return "unknown";
        }
    }

    void dump(GameObject* object) {
        std::ostream& out = std::cout;
        out << "GameObject: {\n";
        out << "\tid: " << object->getId() << "\n";
        out << "\tzOrder: " << object->state.zOrder << "\n";
        out << "\ttype: " << typeName(object->state.type) << "\n";
        out << "\tside: " << sideName(object->state.side) << "\n";
        out << "\tisDead: " << object->state.isDead << "\n";
        out << "\tremoveOnDeath: " << object->state.isToBeRemovedOnDeath << "\n";
        out << "\tposition: " << object->currentState.position.x << ", " << object->currentState.position.y << "\n";
        out << "}";
        out << std::endl;
    }

    void Scene::dumpEntities() {
        std::cout << "===== Dumping game entities " << std::endl;
        for (auto& gameObject : objects)
            dump(gameObject);
    }

    void Scene::clear() {
        objects.clear();
        newlyCreatedObjects.clear();
        collisions.clear();
    }

    void Scene::setBrain(GameObject* object, char const* const brainName) {
        auto brain = objectFactory->getBrain(brainName);
        object->setBrain(brain);
        object->setScene(this);
        brain->onSpawn(object);
    }

    GameObject* Scene::getById(long id) {
        for (auto& o : objects)
            if (o->getId() == id)
                return o;
        
        return nullptr;
    }
}