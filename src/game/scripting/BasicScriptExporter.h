#pragma once

#include <ScriptExporter.h>

namespace Acidrain {

    class BasicScriptExporter : public ScriptExporter {
    public:
        static BasicScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        BasicScriptExporter();
    };
}