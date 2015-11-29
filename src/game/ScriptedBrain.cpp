#include <easylogging++.h>
#include <ScriptedBrain.h>
#include <GameObject.h>
#include <FileSystem.h>
#include <Scene.h>
#include <InputProvider.h>
#include <Camera.h>
#include <AudioSystem.h>
#include <GameEvent.h>
#include <ScriptExporterRegistry.h>
#include <GameSession.h>
#include <GameServiceLocator.h>
#include <ScriptUtils.h>

namespace Acidrain {

    ScriptedBrain::ScriptedBrain(const string& brainFilename)
            : brainName(brainFilename) {
        initializeLuaContext(brainFilename);
    }

    void ScriptedBrain::initializeLuaContext(const string& brainFilename) {
        L = luaL_newstate();
        luaL_openlibs(L);

        ScriptExporterRegistry::exportTo(L);

        Lua::loadAndInterpret(L, "scripts/base.lua");
        Lua::loadAndInterpret(L, brainFilename.c_str());
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
        if (lua_pcall(L, numberOfArguments, numberOfResults, 0) != 0)
            LOG(ERROR) << "Failed to run " << brainName << "#onSpawn. Error: " << lua_tostring(L, -1);
    }

    void ScriptedBrain::onDeath(GameObject* gameObject) {
        lua_getglobal(L, "onDeath");
        lua_pushlightuserdata(L, gameObject);
        lua_pushnumber(L, (int) gameObject->state.deathReason);

        int numberOfArguments = 2;
        int numberOfResults = 0;
        if (lua_pcall(L, numberOfArguments, numberOfResults, 0) != 0)
            LOG(ERROR) << "Failed to run " << brainName << "#onDeath. Error: " << lua_tostring(L, -1);
    }

    void ScriptedBrain::onUpdate(GameObject* gameObject, float elapsedSeconds) {
        lua_getglobal(L, "onUpdate");
        lua_pushlightuserdata(L, gameObject);
        lua_pushnumber(L, elapsedSeconds);

        int numberOfArguments = 2;
        int numberOfResults = 0;
        if (lua_pcall(L, numberOfArguments, numberOfResults, 0) != 0)
            LOG(ERROR) << "Failed to run " << brainName << "#onUpdate. Error: " << lua_tostring(L, -1);
    }

    void ScriptedBrain::onDamage(GameObject* gameObject, float damage, GameObject* damageInflicter) {
        lua_getglobal(L, "onDamage");
        lua_pushlightuserdata(L, gameObject);
        lua_pushnumber(L, damage);
        lua_pushlightuserdata(L, damageInflicter);

        int numberOfArguments = 3;
        int numberOfResults = 0;
        if (lua_pcall(L, numberOfArguments, numberOfResults, 0) != 0)
            LOG(ERROR) << "Failed to run " << brainName << "#onDamage. Error: " << lua_tostring(L, -1);
    }
}