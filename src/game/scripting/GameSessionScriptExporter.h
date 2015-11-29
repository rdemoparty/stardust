#pragma once

#include <ScriptExporter.h>

namespace Acidrain {

    class GameSessionScriptExporter : public ScriptExporter {
    public:
        static GameSessionScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        GameSessionScriptExporter();
    };
}