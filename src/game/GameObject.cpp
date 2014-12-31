#include <GameObject.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>
#include <Weapon.h>
#include <Scene.h>
#include <AudioSystem.h>

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
                    auto bullet = scene->createByName(emitter.bulletType);
                    bullet->currentState.position = position + emitter.mountingPoint;
                    scene->add(bullet);
                }

                if (!weapon->getFireSound().empty())
                    AUDIOSYS.playSound(weapon->getFireSound().c_str(), "PLAYER_WEAPONS");
            }
        }
    }

    void GameObject::inflictDamage(float amount, GameObject* inflicter) {
        state.inflictDamage(amount);
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
}