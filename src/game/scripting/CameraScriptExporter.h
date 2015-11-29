#pragma once

#include <ScriptExporter.h>

namespace Acidrain {

    class CameraScriptExporter : public ScriptExporter {
    public:
        static CameraScriptExporter& getInstance();
        void exportToScript(lua_State* L) const override;

    private:
        CameraScriptExporter();
    };
}