#pragma once

#include <Entity.h>
#include <SpritePool.h>
#include <AttributeBag.h>
#include <ScriptedBrain.h>

namespace Acidrain {

    using namespace std;

    class Animation;

    class MovementStrategy;

    class Weapon;

    struct BulletInfo;

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

    enum class EntityDeathReason {
        OutOfVisibleArea,
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

        void setMovementController(shared_ptr<MovementStrategy> const& movementController);

        void fireWeapons(bool shouldFire);

        void setScene(Scene* scene);

        void setBrain(shared_ptr<ScriptedBrain> brain);

        void addWeapon(Weapon* weapon, const vec2& offset);

        virtual void update(float elapsedSeconds) override;

        void updateAnimation(float elapsedSeconds);

        void inflictDamage(float amount);

        AttributeBag& attributes();

        EntityState state;

    protected:
        AttributeBag attrs;
        shared_ptr<ScriptedBrain> brain;

        friend class GameObjectFactory;

        void updateWeapons(float elapsedSeconds);

        GameObject* newBullet(BulletInfo* info);

        Scene* scene;
        shared_ptr<MovementStrategy> movementController;
        Animation* animation;
        std::map<Weapon*, vec2> weapons;
    };
}
