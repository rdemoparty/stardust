#pragma once

#include <lua.hpp>

namespace Acidrain {

    class ScriptExporter {
    public:

        virtual void exportToScript(lua_State *L) const = 0;
    };
}
