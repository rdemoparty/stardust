#pragma once

#include <ScriptExporter.h>

namespace Acidrain {

    class AudioScriptExporter : public ScriptExporter {
    public:
        static AudioScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        AudioScriptExporter();
    };
}