#pragma once

#include <ScriptExporter.h>

namespace Acidrain {

    class InputScriptExporter : public ScriptExporter {
    public:
        static InputScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        InputScriptExporter();
    };
}