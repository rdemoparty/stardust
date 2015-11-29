#include <DialogRepositoryScriptExporter.h>
#include <DialogRepository.h>
#include <Dialog.h>
#include <easylogging++.h>
#include <ScriptExporterRegistry.h>

#include <ScriptUtils.h>
#include <glm/ext.hpp>

namespace Acidrain {

    using namespace Lua;

    static int luaCreateDialog(lua_State* L) {
        luaL_checktype(L, 1, LUA_TTABLE);

        string message = readTableString(L, "message");
        float x = readTableFloat(L, "x");
        float y = readTableFloat(L, "y");

        Font* font = readTableFont(L, "font");

        lua_getfield(L, -1, "options");
        DialogOptions options = lua_isnil(L, -1) ? DEFAULT_DIALOG_OPTIONS : readDialogOptions(L);
        lua_pop(L, 1); // we're done with the table

        int dialogId = DialogRepository::getInstance().create(message, x, y, font, options);
        lua_pushinteger(L, dialogId);

        return 1; // arguments pushed on stack
    }

    static int luaUpdateDialog(lua_State* L) {
        float dt = readFloat(L);
        lua_pop(L, 1);

        int dialogId = readInteger(L);
        lua_pop(L, 1);

        DialogRepository::getInstance().update(dialogId, dt);

        return 0; // arguments pushed on stack
    }

    static int luaRemoveDialog(lua_State* L) {
        int dialogId = readInteger(L);
        lua_pop(L, 1);

        DialogRepository::getInstance().remove(dialogId);

        return 0; // arguments pushed on stack
    }


    DialogRepositoryScriptExporter& DialogRepositoryScriptExporter::getInstance() {
        static DialogRepositoryScriptExporter instance;
        return instance;
    }

    DialogRepositoryScriptExporter::DialogRepositoryScriptExporter() {
    }

    void DialogRepositoryScriptExporter::exportToScript(lua_State* L) const {
        lua_register(L, "createDialog", luaCreateDialog);
        lua_register(L, "updateDialog", luaUpdateDialog);
        lua_register(L, "removeDialog", luaRemoveDialog);
    }

} // end of namespace Acidrain

