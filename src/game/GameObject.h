#pragma once

#include <Entity.h>
#include <SpritePool.h>
#include <AttributeBag.h>
#include <ScriptedBrain.h>
#include <vector>

namespace Acidrain {
    using namespace std;

    class Animation;
    class Weapon;
    class Scene;
    class ScriptedBrain;

    enum EntitySide {
        Friendly = 1,
        Adverse,
        Neutral
    };

    enum EntityType {
        Scenery = 1,
        Ship,
        Bullet,
        Explosion,
        Bonus
    };

    enum EntityDeathReason {
        OutOfVisibleArea = 1,
        Killed,
        JobFinished
    };

    struct EntityState {
        bool isCollidable;
        int zOrder;
        float damageProvidedOnCollision;
        bool isToBeRemovedOnDeath;
        bool killIfOutsideOfVisibleArea;
        bool isHidden;
        float maxLife;
        float life;
        bool isDead;
        EntityDeathReason deathReason;
        EntityType type;
        EntitySide side;
    };

    class GameObject : public DrawableEntity {
    public:
        virtual ~GameObject() {};

        long getId() const {
            return id;
        }

        void setId(long id) {
            GameObject::id = id;
        }

        void fireWeapons(bool shouldFire);

        void setBrain(shared_ptr<ScriptedBrain> brain);

        void addWeapon(Weapon* weapon);

        virtual void update(float elapsedSeconds);

        void updateAnimation(float elapsedSeconds);

        void inflictDamage(float amount, GameObject* inflicter);

        void kill(EntityDeathReason deathReason);

        AttributeBag& attributes();

        EntityState state;

        void spawned();

        bool isAnimationFinished();

    protected:
        long id;

        AttributeBag attrs;
        shared_ptr<ScriptedBrain> brain;

        friend class GameObjectFactory;

        void updateWeapons(float elapsedSeconds);

        Animation* animation;
        vector<shared_ptr<Weapon>> weapons;
    };
}
