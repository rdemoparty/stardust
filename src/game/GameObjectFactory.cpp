#include <easylogging++.h>
#include <GameObjectFactory.h>
#include <Animation.h>
#include <SpriteAnimationRepository.h>
#include <FileSystem.h>
#include <json11.hpp>
#include <stdexcept>

namespace Acidrain {

    using namespace json11;

    bool icompare(const string& a, const string& b) {
        return a == b;
    }

    GameObjectFactory::GameObjectFactory() {
        initialize("recipes.json");
    }

    GameObjectFactory::~GameObjectFactory() {
    }

    GameObject* GameObjectFactory::createByName(const string& name) {
        GameObject* result = nullptr;
        if (recipes.find(name) != recipes.end()) {
            result = cookGameObject(recipes[name]);
        } else {
            LOG(ERROR) << "Wanting to create object of type \"" << name << "\" but not such a recipe registered";
        }
        return result;
    }

    GameObject* GameObjectFactory::cookGameObject(GameObjectRecipe& recipe) {
        LOG(TRACE) << "Cooking game object of type " << recipe.name;
        GameObject* result = new GameObject();
        result->setId(NEXT_ID++);

        if (!recipe.brain.empty())
            result->setBrain(cookBrain(recipe.brain));

        result->animation = ANIMREPO.newAnimation(recipe.animation);
        result->animation->start();
        result->currentState.size = result->animation->getSprite().getSize();

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
        Weapon* result = new Weapon(recipe.shotsPerSecond, recipe.soundWhenFired);
        for (auto& emitterRecipe : recipe.emitters)
            result->addEmitter({emitterRecipe.bulletName, emitterRecipe.mountingPoint});
        return result;
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
        LOG(INFO) << "Registering entity recipe \"" << recipe.name << "\"";
        recipes[recipe.name] = recipe;
    }

    shared_ptr<ScriptedBrain> GameObjectFactory::getBrain(char const* const brainName) {
        return cookBrain(brainName);
    }

    EntitySide entitySideFromString(const string& param) {
        if (icompare(param, "Friendly"))
            return EntitySide::Friendly;
        else if (icompare(param, "Adverse"))
            return EntitySide::Adverse;
        else if (icompare(param, "Neutral"))
            return EntitySide::Neutral;
        else {
            LOG(FATAL) << "Unknown entity side " << param;
            throw runtime_error("Unknown entity side " + param);
        }
    }

    EntityType entityTypeFromString(const string& value) {
        if (icompare(value, "Scenery"))
            return EntityType::Scenery;
        else if (icompare(value, "Ship"))
            return EntityType::Ship;
        else if (icompare(value, "Bullet"))
            return EntityType::Bullet;
        else if (icompare(value, "Explosion"))
            return EntityType::Explosion;
        else if (icompare(value, "Bonus"))
            return EntityType::Bonus;
        else {
            LOG(FATAL) << "Unknown entity type " << value;
            throw runtime_error("Unknown entity type " + value);
        }
    }

    WeaponEmitterRecipe readWeaponEmitter(const Json& element) {
        WeaponEmitterRecipe result;
        for (auto& e : element.object_items()) {
            const string& param = e.first;
            if (icompare(param, "bulletName"))
                result.bulletName = e.second.string_value();
            else if (icompare(param, "x")) {
                result.mountingPoint.x = (float) e.second.number_value();
            } else if (icompare(param, "y")) {
                result.mountingPoint.y = (float) e.second.number_value();
            } else {
                LOG(WARNING) << "Unknown weapon recipe attribute " << param;
            }
        }
        return result;
    }


    WeaponRecipe readWeapon(const Json& element) {
        WeaponRecipe result;
        for (auto& e : element.object_items()) {
            const string& param = e.first;
            if (icompare(param, "shotsPerSecond"))
                result.shotsPerSecond = e.second.int_value();
            else if (icompare(param, "soundWhenFired")) {
                result.soundWhenFired = e.second.string_value();
            }
            else if (icompare(param, "emitters")) {
                for (auto& k : e.second.array_items())
                    result.emitters.push_back(readWeaponEmitter(k));
            }
            else {
                LOG(WARNING) << "Unknown weapon recipe attribute " << param;
            }
        }
        return result;
    }

    CollisionHullRecipe readCollisionHull(const Json& element) {
        CollisionHullRecipe result;
        for (auto& e : element.object_items()) {
            const string& param = e.first;
            if (icompare(param, "radius"))
                result.radius = (float) e.second.number_value();
            else if (icompare(param, "x"))
                result.center.x = (float) e.second.number_value();
            else if (icompare(param, "y"))
                result.center.y = (float) e.second.number_value();
            else {
                LOG(WARNING) << "Unknown collision hull recipe attribute " << param;
            }
        }
        return result;
    }

    GameObjectRecipe readGameObjectRecipe(const Json& element) {
        GameObjectRecipe result = {};

        for (auto& e : element.object_items()) {
            const string& param = e.first;
            if (icompare(param, "name"))
                result.name = e.second.string_value();
            else if (icompare(param, "animation"))
                result.animation = e.second.string_value();
            else if (icompare(param, "brain"))
                result.brain = e.second.string_value();
            else if (icompare(param, "collidable"))
                result.collidable = e.second.bool_value();
            else if (icompare(param, "damageProvidedOnCollision"))
                result.damageProvidedOnCollision = (float) e.second.number_value();
            else if (icompare(param, "removeOnDeath"))
                result.removeOnDeath = e.second.bool_value();
            else if (icompare(param, "killIfOutside"))
                result.killIfOutside = e.second.bool_value();
            else if (icompare(param, "maxLife"))
                result.maxLife = (float) e.second.number_value();
            else if (icompare(param, "team"))
                result.team = entitySideFromString(e.second.string_value());
            else if (icompare(param, "type"))
                result.type = entityTypeFromString(e.second.string_value());
            else if (icompare(param, "hull"))
                for (auto& k : e.second.array_items())
                    result.hull.push_back(readCollisionHull(k));
            else if (icompare(param, "weapons"))
                for (auto& k : e.second.array_items())
                    result.weapons.push_back(readWeapon(k));
            else {
                LOG(WARNING) << "Unknown game object recipe attribute " << param;
            }
        }

        return result;
    }

    void GameObjectFactory::initialize(string filename) {
        LOG(INFO) << "Loading game objects repository from " << filename;
        string content = FILESYS.getFileContent(filename);

        string parseError;
        auto json = Json::parse(content, parseError);
        if (parseError.empty())
            for (auto& k : json["recipes"].array_items())
                addRecipe(readGameObjectRecipe(k));
        else
            LOG(FATAL) << "Error while parsing JSON content from " << filename << ". Error: " << parseError;
    }
} // namespace Acidrain