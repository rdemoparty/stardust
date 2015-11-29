#pragma once

#include <ScriptExporter.h>

namespace Acidrain {

    class EntityScriptExporter : public ScriptExporter {
    public:
        static EntityScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        EntityScriptExporter();
    };
}