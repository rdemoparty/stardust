#pragma once

#include <ScriptExporter.h>

namespace Acidrain {

    class SceneScriptExporter : public ScriptExporter {
    public:
        static SceneScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        SceneScriptExporter();
    };
}