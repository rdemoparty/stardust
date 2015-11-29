#pragma once
#include <ScriptExporter.h>

namespace Acidrain {

    /**
     * Self registering script exporter. An instance is created automagically behind the scenes which in turn triggers
     * the ScriptExporterRegistry registration process.
     */
    class DialogRepositoryScriptExporter : public ScriptExporter {
    public:
        static DialogRepositoryScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        DialogRepositoryScriptExporter();
    };

} // end of namespace Acidrain