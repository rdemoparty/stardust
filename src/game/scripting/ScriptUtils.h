#pragma once

#include <luaconf.h>
#include <lua.hpp>
#include <Font.h>
#include <Dialog.h>
#include <string.h>
#include <easylogging++.h>

namespace Acidrain {
    namespace Lua {

        void loadAndInterpret(lua_State* L, const char* const scriptUri);
        void dumpStack(lua_State* L);

        inline bool readBoolean(lua_State* L) {
            bool value = lua_toboolean(L, -1);
            lua_pop(L, 1);
            return value;
        }

        inline float readFloat(lua_State* L) {
            float value = (float)luaL_checknumber(L, -1);
            lua_pop(L, 1);
            return value;
        }

        inline int readInteger(lua_State* L) {
            int value = luaL_checkint(L, -1);
            lua_pop(L, 1);
            return value;
        }

        inline string readString(lua_State* L) {
            string value = string(luaL_checkstring(L, -1));
            lua_pop(L, 1);
            return value;
        }

        inline string readTableString(lua_State* L, const char* fieldName) {
            lua_getfield(L, -1, fieldName);
            return readString(L);
        }

        inline float readTableFloat(lua_State* L, const char* fieldName) {
            lua_getfield(L, -1, fieldName);
            return readFloat(L);
        }

        inline vec4 readColor(lua_State* L) {
            vec4 result;

            result.r = readTableFloat(L, "r");
            result.g = readTableFloat(L, "g");
            result.b = readTableFloat(L, "b");
            result.a = readTableFloat(L, "a");

            lua_pop(L, 1);

            return result;
        }

        inline DialogOptions readDialogOptions(lua_State* L) {
            DialogOptions result = DEFAULT_DIALOG_OPTIONS;

            luaL_checktype(L, -1, LUA_TTABLE);
            lua_pushnil(L);
            while (lua_next(L, -2) != 0) {
                const char* key = lua_tostring(L, -2);
                if (strcmp(key, "scrollOldLinesUp") == 0) {
                    result.scrollOldLinesUp = readBoolean(L);
                } else if (strcmp(key, "blinkingCursor") == 0) {
                    result.blinkingCursor = readBoolean(L);
                } else if (strcmp(key, "blinksPerSecond") == 0) {
                    result.blinksPerSecond = readFloat(L);
                } else if (strcmp(key, "showCursor") == 0) {
                    result.showCursor = readBoolean(L);
                } else if (strcmp(key, "cursor") == 0) {
                    result.cursor = readString(L).at(0);
                } else if (strcmp(key, "charactersTypedPerSecond") == 0) {
                    result.charactersTypedPerSecond = readFloat(L);
                } else if (strcmp(key, "charSound") == 0) {
                    result.charSound = readString(L);
                } else if (strcmp(key, "charColor") == 0) {
                    result.charColor = readColor(L);
                } else if (strcmp(key, "activeCharColor") == 0) {
                    result.activeCharColor = readColor(L);
                } else {
                    LOG(WARNING) << "Unknown key " << key << " when reading dialog options";
                    lua_pop(L, 1);
                }
            }
            lua_pop(L, 1);
            return result;
        }

        inline Font* readTableFont(lua_State* L, const char* fieldName) {
            lua_getfield(L, -1, fieldName);
            luaL_checktype(L, -1, LUA_TTABLE);

            string face = readTableString(L, "face");
            float size = readTableFloat(L, "size");

            Font* result = new Font(face, size);
            lua_pop(L, 1);

            return result;
        }
    }

} // end of namespace Acidrain