#include <InputScriptExporter.h>
#include <ScriptUtils.h>
#include <InputProvider.h>

namespace Acidrain {

    InputScriptExporter::InputScriptExporter() {
    }

    InputScriptExporter& InputScriptExporter::getInstance() {
        static InputScriptExporter instance;
        return instance;
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


    void InputScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "tildaJustPressed", tildaJustPressed);
        lua_register(L, "fireJustPressed", fireJustPressed);
        lua_register(L, "fireJustReleased", fireJustReleased);
        lua_register(L, "upPressed", upPressed);
        lua_register(L, "downPressed", downPressed);
        lua_register(L, "leftPressed", leftPressed);
        lua_register(L, "rightPressed", rightPressed);
    }
}