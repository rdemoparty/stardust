#include <GameObject.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>
#include <Weapon.h>
#include <Scene.h>

namespace Acidrain {

    GameObject::GameObject() {
    };

    GameObject::~GameObject() {
    };

    void GameObject::fireWeapons(bool shouldFire) {
        for (auto weapon : weapons) {
            if (shouldFire)
                weapon->fireOn();
            else
                weapon->fireOff();
        }
    }

    void GameObject::addWeapon(Weapon* weapon) {
        weapons.push_back(shared_ptr<Weapon>(weapon));
    }

    void GameObject::setScene(Scene* scene) {
        this->scene = scene;
        if (brain)
            brain->injectScene(scene);
    }

    void GameObject::setBrain(shared_ptr<ScriptedBrain> brain) {
        this->brain = brain;
    }

    void GameObject::update(float elapsedSeconds) {
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
        for (auto weapon : weapons) {
            if (weapon->update(elapsedSeconds)) {
//                std::cout << "Creating a bullet with name " << weapon->getBulletType() << std::endl;
                auto bullet = scene->createByName(weapon->getBulletType());
//                std::cout << "Created pointer " << bullet << std::endl;
                bullet->position = this->position + weapon->getMountingPoint();
                scene->add(bullet);
            }
        }
    }

    void GameObject::inflictDamage(float amount) {
        state.inflictDamage(amount);
        if (state.isDead && brain)
            brain->onDeath(this);
    }

    AttributeBag& GameObject::attributes() {
        return attrs;
    }

    void GameObject::spawned() {
        if (brain)
            brain->onSpawn(this);
    }

    bool GameObject::isAnimationFinished() {
        return animation == nullptr || animation->isFinished();
    }

    void GameObject::kill(EntityDeathReason deathReason) {
        state.isDead = true;
        state.deathReason = deathReason;
        if (brain)
            brain->onDeath(this);
    }
}