#pragma once
#include <ScriptExporter.h>

namespace Acidrain {

    /**
     * Self registering script exporter. An instance is created automagically behind the scenes which in turn triggers
     * the ScriptExporterRegistry registration process.
     */
    class CutSceneScriptExporter : public ScriptExporter {
    public:
        static CutSceneScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        CutSceneScriptExporter();
    };

} // end of namespace Acidrain