#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include <GameObject.h>
#include <Weapon.h>

namespace Acidrain {

    using namespace std;
    using namespace glm;

    class GameObject;

    struct CollisionHullRecipe {
        float radius;
        vec2 center;
    };

    struct WeaponRecipe {
        string bulletName;
        vec2 mountingPoint;
        int shotsPerSecond;
    };

    struct GameObjectRecipe {
        string name;
        string animation;
        string brain;
        bool removeOnDeath;
        bool killIfOutside;
        float damageProvidedOnCollision;
        bool collidable;
        float maxLife;
        EntityType type;
        EntitySide team;
        vector<WeaponRecipe> weapons;
        vector<CollisionHullRecipe> hull;
    };

    class GameObjectFactory {
    public:

        GameObjectFactory();

        ~GameObjectFactory();

        GameObject* createByName(const string& name);

    private:

        long NEXT_ID = 1;

        GameObject* cookGameObject(GameObjectRecipe& recipe);

        Weapon* cookWeapon(WeaponRecipe recipe);

        Circle cookHullPart(CollisionHullRecipe recipe);

        shared_ptr<ScriptedBrain> cookBrain(string brainFilename);

        // --- internal state

        map<string, GameObjectRecipe> recipes;

        map<string, shared_ptr<ScriptedBrain>> brains;

        void addRecipe(GameObjectRecipe recipe);
    };

}