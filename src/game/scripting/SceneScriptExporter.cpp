#include <SceneScriptExporter.h>
#include <easylogging++.h>
#include <ScriptUtils.h>
#include <GameServiceLocator.h>

namespace Acidrain {

    SceneScriptExporter::SceneScriptExporter() {
    }

    SceneScriptExporter& SceneScriptExporter::getInstance() {
        static SceneScriptExporter instance;
        return instance;
    }

    static int confineToPlayingArea(lua_State* L) {
        GameObject* object = (GameObject*) lua_topointer(L, 1);
        lua_pushboolean(L, GameServiceLocator::scene()->confineEntityInVisibleArea(object));
        return 1; // arguments pushed on stack
    }

    static int dumpEntities(lua_State* L) {
        GameServiceLocator::scene()->dumpEntities();
        return 0; // arguments pushed on stack
    }

    static int endLevel(lua_State* L) {
        GameServiceLocator::scene()->queueEvent(GameEvent::LEVEL_END);
        return 0; // arguments pushed on stack
    }

    static int pauseLevel(lua_State* L) {
        GameServiceLocator::scene()->queueEvent(GameEvent::LEVEL_SCROLL_PAUSE);
        return 0; // arguments pushed on stack
    }

    static int resumeLevel(lua_State* L) {
        GameServiceLocator::scene()->queueEvent(GameEvent::LEVEL_SCROLL_RESUME);
        return 0; // arguments pushed on stack
    }

    void SceneScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "confineToPlayingArea", confineToPlayingArea);
        lua_register(L, "dumpEntities", dumpEntities);
        lua_register(L, "endLevel", endLevel);
        lua_register(L, "pauseLevel", pauseLevel);
        lua_register(L, "resumeLevel", resumeLevel);
    }
}