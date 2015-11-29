#include <EntityScriptExporter.h>
#include <easylogging++.h>
#include <ScriptUtils.h>
#include <GameObject.h>
#include <GameServiceLocator.h>

namespace Acidrain {

    EntityScriptExporter::EntityScriptExporter() {
    }

    EntityScriptExporter& EntityScriptExporter::getInstance() {
        static EntityScriptExporter instance;
        return instance;
    }

    static int isAnimationFinished(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushboolean(L, object->isAnimationFinished());
        return 1; // arguments pushed on stack
    }

    static int kill(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        EntityDeathReason deathReason = (EntityDeathReason) lua_tointeger(L, 2);
        object->kill(deathReason);
        return 0; // arguments pushed on stack
    }

    static int killAllChildren(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        EntityDeathReason deathReason = (EntityDeathReason) lua_tointeger(L, 2);
        object->killAllChildren(deathReason);
        return 0; // arguments pushed on stack
    }

    static int provideDamage(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        float amount = (float) lua_tonumber(L, 2);
        GameObject* inflicter = (GameObject*) lua_topointer(L, 3);

        if (object != nullptr)
            object->inflictDamage(amount, inflicter);

        return 0; // arguments pushed on stack
    }

    static int fireWeapons(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->fireWeapons((bool) lua_toboolean(L, 2));
        return 0; // arguments pushed on stack
    }

    static int createEntity(lua_State* L) {
        const char* const key = lua_tostring(L, 1);
        GameObject* parent = (GameObject*) lua_topointer(L, 2);

        GameObject* object = GameServiceLocator::gameObjectFactory()->createByName(key);
        GameServiceLocator::scene()->add(object);

        if (parent != nullptr) {
            object->setParent(parent);
            parent->addChild(object);
        }

        lua_pushlightuserdata(L, object);
        return 1; // arguments pushed on stack
    }

    static int getParent(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushlightuserdata(L, object->getParent());
        return 1; // arguments pushed on stack
    }

    static int hasParent(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushboolean(L, object->getParent() != nullptr);
        return 1; // arguments pushed on stack
    }

    static int getPosition(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->currentState.position.x);
        lua_pushnumber(L, object->currentState.position.y);
        return 2; // arguments pushed on stack
    }

    static int getId(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->getId());
        return 1; // arguments pushed on stack
    }

    static int setPosition(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->currentState.position.x = (float) lua_tonumber(L, 2);
        object->currentState.position.y = (float) lua_tonumber(L, 3);
        return 0; // arguments pushed on stack
    }

    static int getRotation(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->currentState.rotation);
        return 1; // arguments pushed on stack
    }

    static int setRotation(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->currentState.rotation = (float) lua_tonumber(L, 2);
        return 0; // arguments pushed on stack
    }

    static int getScale(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->currentState.scale.x);
        return 1; // arguments pushed on stack
    }

    static int setScale(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->currentState.scale = glm::vec2((float) lua_tonumber(L, 2));
        return 0; // arguments pushed on stack
    }

    static int getLife(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->state.life);
        return 1; // arguments pushed on stack
    }

    static int setLife(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->state.life = (float) lua_tonumber(L, 2);
        return 0; // arguments pushed on stack
    }

    static int getMaxLife(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->state.maxLife);
        return 1; // arguments pushed on stack
    }

    static int setMaxLife(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->state.maxLife = (float) lua_tonumber(L, 2);
        return 0; // arguments pushed on stack
    }

    static int getColor(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->currentState.color.r);
        lua_pushnumber(L, object->currentState.color.g);
        lua_pushnumber(L, object->currentState.color.b);
        lua_pushnumber(L, object->currentState.color.a);
        return 4; // arguments pushed on stack
    }

    static int setColor(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->currentState.color.r = (float) lua_tonumber(L, 2);
        object->currentState.color.g = (float) lua_tonumber(L, 3);
        object->currentState.color.b = (float) lua_tonumber(L, 4);
        object->currentState.color.a = (float) lua_tonumber(L, 5);
        return 0; // arguments pushed on stack
    }

    static int getIntAttribute(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        const char* const key = lua_tostring(L, 2);
        lua_pushnumber(L, object->attributes().getInt(key));
        return 1; // arguments pushed on stack
    }

    static int setIntAttribute(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        const char* const key = lua_tostring(L, 2);
        object->attributes().setInt(key, (int) lua_tonumber(L, 3));
        return 0; // arguments pushed on stack
    }

    static int getFloatAttribute(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        const char* const key = lua_tostring(L, 2);
        lua_pushnumber(L, object->attributes().getFloat(key));
        return 1; // arguments pushed on stack
    }

    static int setFloatAttribute(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        const char* const key = lua_tostring(L, 2);
        object->attributes().setFloat(key, (float) lua_tonumber(L, 3));
        return 0; // arguments pushed on stack
    }

    static int setBrain(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        const char* const brainName = lua_tostring(L, 2);

        auto brain = GameServiceLocator::gameObjectFactory()->getBrain(brainName);
        object->setBrain(brain);
        object->spawned();

        return 0; // arguments pushed on stack
    }

    void EntityScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "fireWeapons", fireWeapons);
        lua_register(L, "kill", kill);
        lua_register(L, "killAllChildren", killAllChildren);
        lua_register(L, "provideDamage", provideDamage);
        lua_register(L, "isAnimationFinished", isAnimationFinished);
        lua_register(L, "getId", getId);
        lua_register(L, "getPosition", getPosition);
        lua_register(L, "setPosition", setPosition);
        lua_register(L, "getRotation", getRotation);
        lua_register(L, "setRotation", setRotation);
        lua_register(L, "getScale", getScale);
        lua_register(L, "setScale", setScale);
        lua_register(L, "getLife", getLife);
        lua_register(L, "setLife", setLife);
        lua_register(L, "getMaxLife", getMaxLife);
        lua_register(L, "setMaxLife", setMaxLife);
        lua_register(L, "getParent", getParent);
        lua_register(L, "hasParent", hasParent);
        lua_register(L, "getColor", getColor);
        lua_register(L, "setColor", setColor);
        lua_register(L, "getInt", getIntAttribute);
        lua_register(L, "setInt", setIntAttribute);
        lua_register(L, "getFloat", getFloatAttribute);
        lua_register(L, "setFloat", setFloatAttribute);
        lua_register(L, "createEntity", createEntity);
        lua_register(L, "setBrain", setBrain);
    }
}