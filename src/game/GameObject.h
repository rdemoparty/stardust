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

    enum class EntitySide {
        Friendly,
        Adverse,
        Neutral
    };

    enum class EntityType {
        Ship,
        Bullet,
        Explosion
    };

    enum EntityDeathReason {
        OutOfVisibleArea = 1,
        Killed,
        JobFinished
    };

    struct EntityState {
        bool isCollidable;
        float damageProvidedOnCollision;
        bool isToBeRemovedOnDeath;
        bool killIfOutsideOfVisibleArea;
        float maxLife;
        float life;
        bool isDead;
        EntityDeathReason deathReason;
        EntityType type;
        EntitySide side;

        bool shouldRemove() {
            return isToBeRemovedOnDeath && isDead;
        }

        void inflictDamage(float amount) {
            life -= amount;
            if (life < 0) {
                isDead = true;
                deathReason = EntityDeathReason::Killed;
            }
        }
    };

    class GameObject : public DrawableEntity {
    public:
        explicit GameObject();

        virtual ~GameObject();

        long getId() const {
            return id;
        }

        void setId(long id) {
            GameObject::id = id;
        }

        void fireWeapons(bool shouldFire);

        void setScene(Scene* scene);

        void setBrain(shared_ptr<ScriptedBrain> brain);

        void addWeapon(Weapon* weapon);

        virtual void update(float elapsedSeconds) override;

        void updateAnimation(float elapsedSeconds);

        void inflictDamage(float amount);

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

        Scene* scene;
        Animation* animation;
        vector<shared_ptr<Weapon>> weapons;
    };
}
