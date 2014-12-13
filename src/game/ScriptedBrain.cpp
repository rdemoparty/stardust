#include <ScriptedBrain.h>
#include <GameObject.h>
#include <iostream>
#include <FileSystem.h>

namespace Acidrain {

    // ----------------------------------------
    // Lua exported functions
    // ----------------------------------------

    static int fireWeapons(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->fireWeapons((bool) lua_toboolean(L, 2));
        return 0; // arguments pushed on stack
    }

    static int getPosition(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->position.x);
        lua_pushnumber(L, object->position.y);
        return 2; // arguments pushed on stack
    }

    static int setPosition(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->position.x = (float)lua_tonumber(L, 2);
        object->position.y = (float)lua_tonumber(L, 3);
        return 0; // arguments pushed on stack
    }

    static int getRotation(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->rotation);
        return 1; // arguments pushed on stack
    }

    static int setRotation(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->rotation = (float)lua_tonumber(L, 2);
        return 0; // arguments pushed on stack
    }

    static int getScale(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->scale.x);
        return 1; // arguments pushed on stack
    }

    static int setScale(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->scale = glm::vec2((float)lua_tonumber(L, 2));
        return 0; // arguments pushed on stack
    }

    static int getColor(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->color.r);
        lua_pushnumber(L, object->color.g);
        lua_pushnumber(L, object->color.b);
        lua_pushnumber(L, object->color.a);
        return 4; // arguments pushed on stack
    }

    static int setColor(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->color.r = (float)lua_tonumber(L, 2);
        object->color.g = (float)lua_tonumber(L, 3);
        object->color.b = (float)lua_tonumber(L, 4);
        object->color.a = (float)lua_tonumber(L, 5);
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

    // ----------------------------------------
    // ScriptedBrain implementation
    // ----------------------------------------

    ScriptedBrain::ScriptedBrain(std::string brainFilename) {
        initializeLuaContext(brainFilename);
        registerExports();
    }

    void ScriptedBrain::initializeLuaContext(string& brainFilename) {
        L = luaL_newstate();
        luaL_openlibs(L);

        if (luaL_loadstring(L, FILESYS.getFileContent(brainFilename).c_str()) != 0)
            cerr << "Failed to load script " << brainFilename << ". Error: " << lua_tostring(L, -1) << endl;

        // Interpret the file so we make the symbols available in the scope (functions)
        if (lua_pcall(L, 0, 0, 0) != 0)
            std::cerr << "Failed to interpret script contents: " << lua_tostring(L, -1) << std::endl;
    }

    void ScriptedBrain::registerExports() {
        lua_register(L, "fireWeapons", fireWeapons);
        lua_register(L, "getPosition", getPosition);
        lua_register(L, "setPosition", setPosition);
        lua_register(L, "getRotation", getRotation);
        lua_register(L, "setRotation", setRotation);
        lua_register(L, "getScale", getScale);
        lua_register(L, "setScale", setScale);
        lua_register(L, "getColor", getColor);
        lua_register(L, "setColor", setColor);
        lua_register(L, "getInt", getIntAttribute);
        lua_register(L, "setInt", setIntAttribute);
        lua_register(L, "getFloat", getFloatAttribute);
        lua_register(L, "setFloat", setFloatAttribute);
    }

    ScriptedBrain::~ScriptedBrain() {
        if (L != nullptr)
            lua_close(L);
    }

    void ScriptedBrain::onSpawn(GameObject* gameObject) {
        callGenericEntityFunction(gameObject, "onSpawn");

    }

    void ScriptedBrain::callGenericEntityFunction(GameObject* gameObject, string functionName) {
        lua_getglobal(L, functionName.c_str());
        lua_pushlightuserdata(L, gameObject);

        int numberOfArguments = 1;
        int numberOfResults = 0;
        if (lua_pcall(L, numberOfArguments, numberOfResults , 0) != 0) {
            cerr << "Error running `" << functionName << "`: " << lua_tostring(L, -1) << endl;
        }
    }

    void ScriptedBrain::onDeath(GameObject* gameObject) {
        callGenericEntityFunction(gameObject, "onDeath");
    }

    void ScriptedBrain::onUpdate(GameObject* gameObject, float elapsedSeconds) {
        lua_getglobal(L, "onUpdate");
        lua_pushlightuserdata(L, gameObject);
        lua_pushnumber(L, elapsedSeconds);

        int numberOfArguments = 2;
        int numberOfResults = 0;
        if (lua_pcall(L, numberOfArguments, numberOfResults , 0) != 0) {
            std::cerr << "Error running `onUpdate`: " << lua_tostring(L, -1) << std::endl;
        }
    }
}