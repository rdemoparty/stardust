#include <GameObjectFactory.h>
#include <MovementStrategyLinear.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>

namespace Acidrain {

    GameObjectRecipe playerBulletRecipe() {
        GameObjectRecipe recipe;
        recipe.name = "player.bullet";
        recipe.animation = "laser.red";
        recipe.brain = "scripts/brain.player.bullet.lua";
        recipe.collidable = true;
        recipe.damageProvidedOnCollision = 2000;
        recipe.removeOnDeath = true;
        recipe.killIfOutside = true;
        recipe.maxLife = 1;
        recipe.team = EntitySide::Friendly;
        recipe.type = EntityType::Bullet;
        recipe.hull.push_back({4, {0, -10}});
        recipe.hull.push_back({4, {0, 10}});

        return recipe;
    }

    GameObjectRecipe enemyBulletRecipe() {
        GameObjectRecipe recipe;
        recipe.name = "enemy.bullet";
        recipe.animation = "laser.blue";
        recipe.brain = "scripts/brain.enemy.bullet.lua";
        recipe.collidable = true;
        recipe.damageProvidedOnCollision = 1;
        recipe.removeOnDeath = true;
        recipe.killIfOutside = true;
        recipe.maxLife = 1;
        recipe.team = EntitySide::Adverse;
        recipe.type = EntityType::Bullet;
        recipe.hull.push_back({4, {0, -10}});
        recipe.hull.push_back({4, {0, 10}});

        return recipe;
    }

    GameObjectRecipe explosionRecipe() {
        GameObjectRecipe recipe;
        recipe.name = "enemy.death.explosion";
        recipe.animation = "explosion";
        recipe.brain = "scripts/brain.enemy.explosion.lua";
        recipe.collidable = false;
        recipe.killIfOutside = false;
        recipe.team = EntitySide::Neutral;
        recipe.type = EntityType::Explosion;

        return recipe;
    }

    GameObjectRecipe playerRecipe() {
        GameObjectRecipe recipe;
        recipe.name = "player";
        recipe.animation = "player";
        recipe.brain = "scripts/brain.player.lua";
        recipe.collidable = true;
        recipe.damageProvidedOnCollision = 10000;
        recipe.removeOnDeath = false;
        recipe.killIfOutside = false;
        recipe.maxLife = 200;
        recipe.team = EntitySide::Friendly;
        recipe.type = EntityType::Ship;

        recipe.hull.push_back({16, {0, 0}});

        recipe.weapons.push_back({"player.bullet", {-10, -32}, 10});
        recipe.weapons.push_back({"player.bullet", {12, -32}, 10});

        return recipe;
    }

    GameObjectRecipe enemyRecipe() {
        GameObjectRecipe recipe;
        recipe.name = "enemy";
        recipe.animation = "enemy2";
        recipe.brain = "scripts/brain.enemy.lua";
        recipe.collidable = true;
        recipe.damageProvidedOnCollision = 10;
        recipe.removeOnDeath = true;
        recipe.killIfOutside = false;
        recipe.maxLife = 100;
        recipe.team = EntitySide::Adverse;
        recipe.type = EntityType::Ship;

        recipe.hull.push_back({32, {0, 0}});

        recipe.weapons.push_back({"enemy.bullet", {0, 32}, 2});
        return recipe;
    }

    GameObjectFactory::GameObjectFactory() {
        addRecipe(playerBulletRecipe());
        addRecipe(enemyRecipe());
        addRecipe(enemyBulletRecipe());
        addRecipe(explosionRecipe());
        addRecipe(playerRecipe());
    }

    GameObjectFactory::~GameObjectFactory() {
    }

    GameObject* GameObjectFactory::createByName(const string& name) {
        GameObject* result = nullptr;
        if (recipes.find(name) != recipes.end()) {
            result = cookGameObject(recipes[name]);
        }
        return result;
    }

    GameObject* GameObjectFactory::cookGameObject(GameObjectRecipe& recipe) {
        GameObject* result = new GameObject();
        if (!recipe.brain.empty())
            result->setBrain(cookBrain(recipe.brain));

        result->animation = ANIMREPO.newAnimation(recipe.animation);
        result->animation->start();
        result->size = result->animation->getSprite().getSize();

        result->state.isToBeRemovedOnDeath = recipe.removeOnDeath;
        result->state.killIfOutsideOfVisibleArea = recipe.killIfOutside;
        result->state.damageProvidedOnCollision = recipe.damageProvidedOnCollision;
        result->state.isCollidable = recipe.collidable;
        result->state.isDead = false;
        result->state.life = recipe.maxLife;
        result->state.maxLife = recipe.maxLife;
        result->state.type = recipe.type;
        result->state.side = recipe.team;

        for (auto weaponRecipe : recipe.weapons)
            result->addWeapon(cookWeapon(weaponRecipe));

        for (auto hullPart : recipe.hull)
            result->collisionHull.add(cookHullPart(hullPart));

        return result;
    }

    Weapon* GameObjectFactory::cookWeapon(WeaponRecipe recipe) {
        return new Weapon(recipe.bulletName, recipe.shotsPerSecond, recipe.mountingPoint);
    }

    Circle GameObjectFactory::cookHullPart(CollisionHullRecipe recipe) {
        return Circle(recipe.radius, recipe.center);
    }

    shared_ptr<ScriptedBrain> GameObjectFactory::cookBrain(string brainFilename) {
        if (brains.find(brainFilename) == brains.end()) {
            brains[brainFilename] = make_shared<ScriptedBrain>(brainFilename);
        }
        return brains[brainFilename];
    }

    void GameObjectFactory::addRecipe(GameObjectRecipe recipe) {
        recipes[recipe.name] = recipe;
    }
}