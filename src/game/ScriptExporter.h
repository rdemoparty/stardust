#pragma once

struct lua_State;

namespace Acidrain {
    class ScriptExporter {
    public:
        virtual void exportToScript(lua_State* L) const = 0;
    };
}
