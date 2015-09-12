#pragma once

#include <lua.hpp>

namespace Acidrain {

    class ScriptExporter;

    class ScriptExporterRegistry {
    public:
        static void add(const ScriptExporter *exporter);

        static void exportTo(lua_State *L);
    };
} // end of namespace Acidrain
