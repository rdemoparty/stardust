#include <GameObject.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>
#include <MovementStrategy.h>
#include <Weapon.h>
#include <Scene.h>
#include <iostream>

namespace Acidrain {

    GameObject::GameObject() {
    };

    GameObject::~GameObject() {
    };

    void GameObject::setMovementController(shared_ptr<MovementStrategy> const& movementController) {
        this->movementController = movementController;
        movementController->setControlledEntity(this);
    }

    void GameObject::fireWeapons(bool shouldFire) {
        for (auto weaponOffsetPair : weapons) {
            if (shouldFire)
                weaponOffsetPair.first->fireOn();
            else
                weaponOffsetPair.first->fireOff();
        }
    }

    void GameObject::addWeapon(Weapon* weapon, const vec2& offset) {
        weapons[weapon] = offset;
    }

    void GameObject::setScene(Scene* scene) {
        this->scene = scene;
    }

    void GameObject::setBrain(shared_ptr<ScriptedBrain> brain) {
        this->brain = brain;
    }

    void GameObject::update(float elapsedSeconds) {
        if (movementController)
            movementController->update(elapsedSeconds);

        if (brain)
            brain->onUpdate(this, elapsedSeconds);

        // update xform
        Entity::update(elapsedSeconds);
        updateWeapons(elapsedSeconds);
    }

    void GameObject::updateAnimation(float elapsedSeconds) {
        animation->update(elapsedSeconds);
        currentSprite = animation->getSprite();
    }

    void GameObject::updateWeapons(float elapsedSeconds) {
        vector<BulletInfo> bullets;

        for (auto& weaponOffsetPair : weapons) {
            bullets = weaponOffsetPair.first->update(elapsedSeconds);

            for (auto bullet : bullets) {
                GameObject* bulletObject = newBullet(&bullet);
                bulletObject->position = this->position + weaponOffsetPair.second;
                scene->add(bulletObject);
            }
        }
    }

    GameObject* GameObject::newBullet(BulletInfo* info) {
        // switch from bullet name
        GameObject* bullet = scene->createByName(info->gameObjectName);
        return bullet;
    }


    void GameObject::inflictDamage(float amount) {
        state.inflictDamage(amount);
        if (state.isDead && state.type == EntityType::Ship) {
            GameObject* explosion = scene->createByName("explosion");
            explosion->position = position;
            scene->add(explosion);
        }
    }

    AttributeBag& GameObject::attributes() {
        return attrs;
    }
}