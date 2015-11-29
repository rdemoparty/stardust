#include <Script.h>
#include <ScriptExporterRegistry.h>
#include <ScriptUtils.h>

namespace Acidrain {

    Script::Script(const char* const scriptUri) {
        L = luaL_newstate();
        luaL_openlibs(L);
        ScriptExporterRegistry::exportTo(L);

        Lua::loadAndInterpret(L, scriptUri);
    }

    Script::~Script() {
        lua_close(L);
    }
}