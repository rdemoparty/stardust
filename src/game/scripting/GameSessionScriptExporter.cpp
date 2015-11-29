#include <GameSessionScriptExporter.h>
#include <ScriptUtils.h>
#include <GameSession.h>
#include <GameServiceLocator.h>

namespace Acidrain {

    GameSessionScriptExporter::GameSessionScriptExporter() {
    }

    GameSessionScriptExporter& GameSessionScriptExporter::getInstance() {
        static GameSessionScriptExporter instance;
        return instance;
    }

    static int luaGetSessionIntAttribute(lua_State* L) {
        lua_pushnumber(L, GameServiceLocator::gameSession()->getSessionAttributes().getInt(Lua::readString(L).c_str()));
        return 1; // arguments pushed on stack
    }

    static int luaSetSessionIntAttribute(lua_State* L) {
        int value = Lua::readInteger(L);
        string key = Lua::readString(L);
        GameServiceLocator::gameSession()->getSessionAttributes().setInt(
                key.c_str(),
                value
        );
        return 0; // arguments pushed on stack
    }

    static int luaGetSessionFloatAttribute(lua_State* L) {
        lua_pushnumber(L, GameServiceLocator::gameSession()->getSessionAttributes().getFloat(Lua::readString(L).c_str()));
        return 1; // arguments pushed on stack
    }

    static int luaSetSessionFloatAttribute(lua_State* L) {
        float value = Lua::readFloat(L);
        string key = Lua::readString(L);
        GameServiceLocator::gameSession()->getSessionAttributes().setFloat(key.c_str(), value);
        return 0; // arguments pushed on stack
    }

    void GameSessionScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "getSessionInt", luaGetSessionIntAttribute);
        lua_register(L, "setSessionInt", luaSetSessionIntAttribute);
        lua_register(L, "getSessionFloat", luaGetSessionFloatAttribute);
        lua_register(L, "setSessionFloat", luaSetSessionFloatAttribute);
    }
}