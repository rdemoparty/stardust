#pragma once

#include <Entity.h>
#include <SpritePool.h>

namespace Acidrain {

    using namespace std;

    class Animation;

    class MovementStrategy;

    class Weapon;

    class BulletInfo;

    class Scene;

    class GameObject : public DrawableEntity {
    public:
        explicit GameObject();

        virtual ~GameObject();

        void setMovementController(shared_ptr<MovementStrategy> const& movementController);

        void fireWeapons(bool shouldFire);

        void setScene(Scene* scene);

        void addWeapon(Weapon* weapon, const vec2& offset);

        virtual void update(float elapsedSeconds) override;

        void updateAnimation(float elapsedSeconds);

    protected:
        friend class GameObjectFactory;

        void updateWeapons(float elapsedSeconds);

        GameObject* newBullet(BulletInfo* info);

        Scene* scene;
        shared_ptr<MovementStrategy> movementController;
        Animation* animation;
        std::map<Weapon*, vec2> weapons;
    };
}