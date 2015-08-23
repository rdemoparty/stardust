#include <easylogging++.h>
#include <ScriptedBrain.h>
#include <GameObject.h>
#include <FileSystem.h>
#include <Scene.h>
#include <InputProvider.h>
#include <Camera.h>
#include <AudioSystem.h>
#include <GameSession.h>
#include <GameEvent.h>

namespace Acidrain {

    // ----------------------------------------
    // Lua exported functions
    // ----------------------------------------

    static void loadAndInterpret(lua_State* L, const char* scriptUri) {
        if (luaL_loadstring(L, FILESYS.getFileContent(scriptUri).c_str()) != 0)
            LOG(ERROR) << "Failed to load script " << scriptUri << ". Error: " << lua_tostring(L, -1) << endl;

        // Interpret the file so we make the symbols available in the scope (functions)
        if (lua_pcall(L, 0, 0, 0) != 0)
            LOG(ERROR) << "Failed to interpret script contents: " << lua_tostring(L, -1) << endl;
    }

    static int includeScriptFile(lua_State* L) {
        loadAndInterpret(L, lua_tostring(L, 1));
        return 0; // arguments pushed on stack
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

    static int tildaJustPressed(lua_State* L) {
        lua_pushboolean(L, INPUT.isKeyJustPressed(SDL_SCANCODE_GRAVE));
        return 1; // arguments pushed on stack
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

    static int dumpEntities(lua_State* L) {
        Scene* scene = (Scene*) lua_topointer(L, 1);
        scene->dumpEntities();
        return 0; // arguments pushed on stack
    }

    static int endLevel(lua_State* L) {
        Scene* scene = (Scene*) lua_topointer(L, 1);
        scene->queueEvent(GameEvent::LEVEL_END);
        return 0; // arguments pushed on stack
    }

    static int shakeCamera(lua_State* L) {
        Scene* scene = (Scene*) lua_topointer(L, 1);
        scene->shakeCamera((float) lua_tonumber(L, 2));
        return 0; // arguments pushed on stack
    }

    static int setBrain(lua_State* L) {
        Scene* scene = (Scene*) lua_topointer(L, 1);
        GameObject* object = (GameObject*) lua_topointer(L, 2);
        const char* const brainName = lua_tostring(L, 3);
        scene->changeObjectBrain(object, brainName);
        return 0; // arguments pushed on stack
    }

    static int playSound(lua_State* L) {
        const char* const URI = lua_tostring(L, 1);
        const char* const AUDIO_GROUP = lua_tostring(L, 2);
        AUDIOSYS.playSound(URI, AUDIO_GROUP);
        return 0; // arguments pushed on stack
    }

    static int playSong(lua_State* L) {
        const char* const uri = lua_tostring(L, 1);
        AUDIOSYS.playMusic(uri);
        return 0; // arguments pushed on stack
    }

    // ----------------------------------------
    // ScriptedBrain implementation
    // ----------------------------------------

    ScriptedBrain::ScriptedBrain(const string& brainFilename)
            : brainName(brainFilename) {
        initializeLuaContext(brainFilename);
    }

    void ScriptedBrain::initializeLuaContext(const string& brainFilename) {
        L = luaL_newstate();
        luaL_openlibs(L);

        registerExports();

        loadAndInterpret(L, "scripts/base.lua");
        loadAndInterpret(L, brainFilename.c_str());
    }

    void ScriptedBrain::registerExports() {
        lua_register(L, "include", includeScriptFile);
        lua_register(L, "fireWeapons", fireWeapons);
        lua_register(L, "kill", kill);
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
        lua_register(L, "getColor", getColor);
        lua_register(L, "setColor", setColor);
        lua_register(L, "getInt", getIntAttribute);
        lua_register(L, "setInt", setIntAttribute);
        lua_register(L, "getFloat", getFloatAttribute);
        lua_register(L, "setFloat", setFloatAttribute);
        lua_register(L, "createEntity", createEntity);
        lua_register(L, "addEntity", addEntity);
        lua_register(L, "tildaJustPressed", tildaJustPressed);
        lua_register(L, "fireJustPressed", fireJustPressed);
        lua_register(L, "fireJustReleased", fireJustReleased);
        lua_register(L, "upPressed", upPressed);
        lua_register(L, "downPressed", downPressed);
        lua_register(L, "leftPressed", leftPressed);
        lua_register(L, "rightPressed", rightPressed);
        lua_register(L, "confineToPlayingArea", confineToPlayingArea);
        lua_register(L, "dumpEntities", dumpEntities);
        lua_register(L, "endLevel", endLevel);
        lua_register(L, "shakeCamera", shakeCamera);
        lua_register(L, "setBrain", setBrain);
        lua_register(L, "playSound", playSound);
        lua_register(L, "playSong", playSong);
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