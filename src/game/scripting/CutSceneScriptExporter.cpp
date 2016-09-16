#include <CutSceneScriptExporter.h>
#include <CutScene.h>
#include <easylogging++.h>
#include <ScriptExporterRegistry.h>
#include <GfxSystem.h>

#include <ScriptUtils.h>

namespace Acidrain {

    using namespace Lua;

    shared_ptr<CutScene> readCutSceneFromLuaStack(lua_State* L);
    vector<shared_ptr<Slide>> readCutSceneSlidesFromLuaStack(lua_State* L);
    shared_ptr<Slide> readSlideFromLuaStack(lua_State* L);
    shared_ptr<Caption> readCaptionFromLuaStack(lua_State* L);
    vector<shared_ptr<Caption>> readCaptionsFromLuaStack(lua_State* L);

    static int luaAddCutSceneBeforeLevel(lua_State* L) {
        int levelNumber = readInteger(L);

        CutSceneRegistry::registerCutScene(readCutSceneFromLuaStack(L),
                                           levelNumber,
                                           CutScenePosition::BeforeLevel);

        return 0; // arguments pushed on stack
    }

    static int luaAddCutSceneAfterLevel(lua_State* L) {
        int levelNumber = readInteger(L);

        CutSceneRegistry::registerCutScene(readCutSceneFromLuaStack(L),
                                           levelNumber,
                                           CutScenePosition::AfterLevel);

        return 0; // arguments pushed on stack
    }

    shared_ptr<CutScene> readCutSceneFromLuaStack(lua_State* L) {
        shared_ptr<CutScene> cutScene = make_shared<CutScene>();

        luaL_checktype(L, -1, LUA_TTABLE);
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            const char* key = lua_tostring(L, -2);
            if (strcmp(key, "music") == 0) {
                cutScene->musicUri = readString(L);
            } else if (strcmp(key, "speech") == 0) {
                cutScene->speechUri = readString(L);
            } else if (strcmp(key, "skippable") == 0) {
                cutScene->skippable = readBoolean(L);
            } else if (strcmp(key, "slides") == 0) {
                cutScene->slides = readCutSceneSlidesFromLuaStack(L);
            } else {
                LOG(WARNING) << "Unhandled key when reading cutscene: " << key;
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);
        return cutScene;
    }

    vector<shared_ptr<Slide>> readCutSceneSlidesFromLuaStack(lua_State* L) {
        vector<shared_ptr<Slide>> slides;

        luaL_checktype(L, -1, LUA_TTABLE);
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            slides.push_back(readSlideFromLuaStack(L));
        }

        lua_pop(L, 1);
        return slides;
    }

    shared_ptr<Slide> readSlideFromLuaStack(lua_State* L) {
        shared_ptr<Slide> slide = make_shared<Slide>();

        luaL_checktype(L, -1, LUA_TTABLE);
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            const char* key = lua_tostring(L, -2);
            if (strcmp("image", key) == 0) {
                slide->texture = GFXSYS.loadTexture(readString(L));
            } else if (strcmp("timeOnSeconds", key) == 0) {
                slide->seconds = readFloat(L);
            } else if (strcmp("fadeInSeconds", key) == 0) {
                slide->fadeInSeconds = readFloat(L);
            } else if (strcmp("fadeOutSeconds", key) == 0) {
                slide->fadeOutSeconds = readFloat(L);
            } else if (strcmp("captions", key) == 0) {
                slide->captions = readCaptionsFromLuaStack(L);
            } else {
                LOG(WARNING) << "Unhandled key when reading cutscene slide: " << key;
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);

        return slide;
    }

    vector<shared_ptr<Caption>> readCaptionsFromLuaStack(lua_State* L) {
        vector<shared_ptr<Caption>> captions;

        luaL_checktype(L, -1, LUA_TTABLE);
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            captions.push_back(readCaptionFromLuaStack(L));
        }

        lua_pop(L, 1);
        return captions;
    }

    shared_ptr<Caption> readCaptionFromLuaStack(lua_State* L) {
        shared_ptr<Caption> caption = make_shared<Caption>();

        luaL_checktype(L, -1, LUA_TTABLE);
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            const char* key = lua_tostring(L, -2);
            if (strcmp("text", key) == 0) {
                caption->content = readString(L);
            } else if (strcmp("delaySeconds", key) == 0) {
                caption->withDelayFromSlideStart(readFloat(L));
            } else if (strcmp("timeOnSeconds", key) == 0) {
                caption->activeForSeconds(readFloat(L));
            } else if (strcmp("fadeInSeconds", key) == 0) {
                caption->fadeInSeconds = readFloat(L);
            } else if (strcmp("fadeOutSeconds", key) == 0) {
                caption->fadeOutSeconds = readFloat(L);
            } else if (strcmp("position", key) == 0) {
                caption->position = readPosition(L);
            } else if (strcmp("color", key) == 0) {
                caption->color = readColor(L);
            } else {
                LOG(WARNING) << "Unhandled key when reading caption: " << key;
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);

        return caption;
    }

    CutSceneScriptExporter& CutSceneScriptExporter::getInstance() {
        static CutSceneScriptExporter instance;
        return instance;
    }

    CutSceneScriptExporter::CutSceneScriptExporter() {
    }

    void CutSceneScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "addCutSceneBeforeLevel", luaAddCutSceneBeforeLevel);
        lua_register(L, "addCutSceneAfterLevel", luaAddCutSceneAfterLevel);
    }
} // end of namespace Acidrain

