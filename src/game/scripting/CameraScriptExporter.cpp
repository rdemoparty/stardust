#include <CameraScriptExporter.h>
#include <easylogging++.h>
#include <ScriptUtils.h>
#include <GameServiceLocator.h>

namespace Acidrain {

    CameraScriptExporter::CameraScriptExporter() {
    }

    CameraScriptExporter& CameraScriptExporter::getInstance() {
        static CameraScriptExporter instance;
        return instance;
    }

    static int shakeCamera(lua_State* L) {
        GameServiceLocator::camera()->shake((float) lua_tonumber(L, 1));
        return 0; // arguments pushed on stack
    }


    void CameraScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "shakeCamera", shakeCamera);
    }
}