#include <GameObject.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>
#include <Weapon.h>
#include <Scene.h>
#include <AudioSystem.h>
#include <GameServiceLocator.h>
#include <algorithm>

namespace Acidrain {

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

    void GameObject::setBrain(shared_ptr<ScriptedBrain> brain) {
        this->brain = brain;
    }

    void GameObject::update(float elapsedSeconds) {
        previousState = currentState;
        if (brain)
            brain->onUpdate(this, elapsedSeconds);

        updateWeapons(elapsedSeconds);
        collisionHull.transform(currentState.getXformMatrix());
    }

    void GameObject::updateAnimation(float elapsedSeconds) {
        animation->update(elapsedSeconds);
        currentSprite = animation->getSprite();
    }

    void GameObject::updateWeapons(float elapsedSeconds) {
        const vec2& position = this->currentState.position;

        for (auto weapon : weapons) {
            if (weapon->update(elapsedSeconds)) {

                const vector<WeaponEmitter>& bulletEmitters = weapon->getEmitters();
                for (auto& emitter : bulletEmitters) {
                    auto bullet = GameServiceLocator::gameObjectFactory()->createByName(emitter.bulletType);
                    bullet->currentState.position = position + emitter.mountingPoint;
                    GameServiceLocator::scene()->add(bullet);
                }

                if (!weapon->getFireSound().empty())
                    AUDIOSYS.playSound(weapon->getFireSound().c_str(), "PLAYER_WEAPONS");
            }
        }
    }

    void GameObject::inflictDamage(float amount, GameObject* inflicter) {
        if (brain)
            brain->onDamage(this, amount, inflicter);

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

    void GameObject::setParent(GameObject* object) {
        this->parent = object;
    }

    GameObject* GameObject::getParent() const {
        return this->parent;
    }

    void GameObject::addChild(GameObject* child) {
        children.push_back(child);
    }

    vector<GameObject*> GameObject::getChildren() {
        return children;
    }

    GameObject::~GameObject() {
        if (parent != nullptr) {
            parent->removeChild(this);
        }

        for (auto child : children) {
            child->setParent(nullptr);
        }
    }

    void GameObject::removeChild(GameObject* child) {
        auto it = find(children.begin(), children.end(), child);
        if (it != children.end()) {
            children.erase(it);
        }
    }

    void GameObject::killAllChildren(EntityDeathReason deathReason) {
        for (auto child : children) {
            child->kill(deathReason);
        }
    }
}