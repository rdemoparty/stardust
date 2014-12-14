#include <ScriptedBrain.h>
#include <GameObject.h>
#include <iostream>
#include <FileSystem.h>
#include <Scene.h>
#include <InputProvider.h>

namespace Acidrain {

    // ----------------------------------------
    // Lua exported functions
    // ----------------------------------------

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

    static int fireWeapons(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->fireWeapons((bool) lua_toboolean(L, 2));
        return 0; // arguments pushed on stack
    }

    static int createEntity(lua_State* L) {
        Scene* scene = (Scene*) lua_topointer(L, 1);
        const char* const key = lua_tostring(L, 2);

        GameObject* object = scene->createByName(key);
        lua_pushlightuserdata(L, object);
        return 1; // arguments pushed on stack
    }

    static int addEntity(lua_State* L) {
        Scene* scene = (Scene*) lua_topointer(L, 1);
        GameObject* object = (GameObject*) lua_topointer(L, 2);

        scene->add(object);

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
        object->position.x = (float) lua_tonumber(L, 2);
        object->position.y = (float) lua_tonumber(L, 3);
        return 0; // arguments pushed on stack
    }

    static int getRotation(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->rotation);
        return 1; // arguments pushed on stack
    }

    static int setRotation(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->rotation = (float) lua_tonumber(L, 2);
        return 0; // arguments pushed on stack
    }

    static int getScale(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushnumber(L, object->scale.x);
        return 1; // arguments pushed on stack
    }

    static int setScale(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        object->scale = glm::vec2((float) lua_tonumber(L, 2));
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
        object->color.r = (float) lua_tonumber(L, 2);
        object->color.g = (float) lua_tonumber(L, 3);
        object->color.b = (float) lua_tonumber(L, 4);
        object->color.a = (float) lua_tonumber(L, 5);
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

    static int fireJustPressed(lua_State* L) {
        lua_pushboolean(L, INPUT.isKeyJustPressed(SDL_SCANCODE_SPACE));
        return 1; // arguments pushed on stack
    }

    static int fireJustReleased(lua_State* L) {
        lua_pushboolean(L, INPUT.isKeyJustReleased(SDL_SCANCODE_SPACE));
        return 1; // arguments pushed on stack
    }

    static int upPressed(lua_State* L) {
        lua_pushboolean(L, INPUT.isKeyDown(SDL_SCANCODE_UP));
        return 1; // arguments pushed on stack
    }

    static int downPressed(lua_State* L) {
        lua_pushboolean(L, INPUT.isKeyDown(SDL_SCANCODE_DOWN));
        return 1; // arguments pushed on stack
    }

    static int leftPressed(lua_State* L) {
        lua_pushboolean(L, INPUT.isKeyDown(SDL_SCANCODE_LEFT));
        return 1; // arguments pushed on stack
    }

    static int rightPressed(lua_State* L) {
        lua_pushboolean(L, INPUT.isKeyDown(SDL_SCANCODE_RIGHT));
        return 1; // arguments pushed on stack
    }

    static int confineToPlayingArea(lua_State* L) {
        Scene* scene = (Scene*) lua_topointer(L, 1);
        GameObject* object = (GameObject*) lua_topointer(L, 2);
        lua_pushboolean(L, scene->confineEntityInVisibleArea(object));
        return 1; // arguments pushed on stack
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

        if (luaL_loadstring(L, FILESYS.getFileContent("scripts/base.lua").c_str()) != 0)
            cerr << "Failed to load base script. Error: " << lua_tostring(L, -1) << endl;

        // Interpret the file so we make the symbols available in the scope (functions)
        if (lua_pcall(L, 0, 0, 0) != 0)
            std::cerr << "Failed to interpret script contents: " << lua_tostring(L, -1) << std::endl;

        if (luaL_loadstring(L, FILESYS.getFileContent(brainFilename).c_str()) != 0)
            cerr << "Failed to load script " << brainFilename << ". Error: " << lua_tostring(L, -1) << endl;

        // Interpret the file so we make the symbols available in the scope (functions)
        if (lua_pcall(L, 0, 0, 0) != 0)
            std::cerr << "Failed to interpret script contents: " << lua_tostring(L, -1) << std::endl;
    }

    void ScriptedBrain::registerExports() {
        lua_register(L, "fireWeapons", fireWeapons);
        lua_register(L, "kill", kill);
        lua_register(L, "isAnimationFinished", isAnimationFinished);
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
        lua_register(L, "createEntity", createEntity);
        lua_register(L, "addEntity", addEntity);
        lua_register(L, "fireJustPressed", fireJustPressed);
        lua_register(L, "fireJustReleased", fireJustReleased);
        lua_register(L, "upPressed", upPressed);
        lua_register(L, "downPressed", downPressed);
        lua_register(L, "leftPressed", leftPressed);
        lua_register(L, "rightPressed", rightPressed);
        lua_register(L, "confineToPlayingArea", confineToPlayingArea);
    }

    void ScriptedBrain::injectScene(Scene* scene) {
        lua_pushlightuserdata(L, scene);
        lua_setglobal(L, "SCENE");
    }

    ScriptedBrain::~ScriptedBrain() {
        if (L != nullptr)
            lua_close(L);
    }

    void ScriptedBrain::onSpawn(GameObject* gameObject) {
        string functionName = "onSpawn";
        lua_getglobal(L, functionName.c_str());
        lua_pushlightuserdata(L, gameObject);

        int numberOfArguments = 1;
        int numberOfResults = 0;
        if (lua_pcall(L, numberOfArguments, numberOfResults, 0) != 0) {
            cerr << "Error running `" << functionName << "`: " << lua_tostring(L, -1) << endl;
        }
    }

    void ScriptedBrain::onDeath(GameObject* gameObject) {
        lua_getglobal(L, "onDeath");
        lua_pushlightuserdata(L, gameObject);
        lua_pushnumber(L, (int) gameObject->state.deathReason);

        int numberOfArguments = 2;
        int numberOfResults = 0;
        if (lua_pcall(L, numberOfArguments, numberOfResults, 0) != 0) {
            std::cerr << "Error running `onDeath`: " << lua_tostring(L, -1) << std::endl;
        }
    }

    void ScriptedBrain::onUpdate(GameObject* gameObject, float elapsedSeconds) {
        lua_getglobal(L, "onUpdate");
        lua_pushlightuserdata(L, gameObject);
        lua_pushnumber(L, elapsedSeconds);

        int numberOfArguments = 2;
        int numberOfResults = 0;
        if (lua_pcall(L, numberOfArguments, numberOfResults, 0) != 0) {
            std::cerr << "Error running `onUpdate`: " << lua_tostring(L, -1) << std::endl;
        }
    }
}