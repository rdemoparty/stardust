#include <BasicScriptExporter.h>
#include <easylogging++.h>
#include <ScriptUtils.h>

namespace Acidrain {

    BasicScriptExporter::BasicScriptExporter() {
    }

    BasicScriptExporter& BasicScriptExporter::getInstance() {
        static BasicScriptExporter instance;
        return instance;
    }

    static int luaIncludeScriptFile(lua_State* L) {
        Lua::loadAndInterpret(L, lua_tostring(L, 1));
        return 0; // arguments pushed on stack
    }

    void BasicScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "include", luaIncludeScriptFile);
    }
}