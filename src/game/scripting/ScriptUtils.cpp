#include <ScriptUtils.h>
#include <FileSystem.h>
#include <easylogging++.h>

namespace Acidrain {
    namespace Lua {

        void loadAndInterpret(lua_State* L, const char* const scriptUri) {
            if (luaL_loadstring(L, FILESYS.getFileContent(scriptUri).c_str()) != 0)
                LOG(ERROR) << "Failed to load script " << scriptUri << ". Error: " << lua_tostring(L, -1) << endl;

            // Interpret the file so we make the symbols available in the scope (functions)
            if (lua_pcall(L, 0, 0, 0) != 0)
                LOG(ERROR) << "Failed to interpret script contents: " << lua_tostring(L, -1) << endl;
        }

        // TODO Adrian: be consistent and switch from using printf() to sstream + LOG(INFO)
        void dumpStack(lua_State* L, const char* title) {
            int i = lua_gettop(L);
            LOG(INFO) << " ----------------  " << title << " ----------------";
            while (i) {
                int t = lua_type(L, i);
                switch (t) {
                    case LUA_TSTRING:
                        printf("\t\t\t%d:`%s'", i, lua_tostring(L, i));
                        break;
                    case LUA_TBOOLEAN:
                        printf("\t\t\t%d: %s", i, lua_toboolean(L, i) ? "true" : "false");
                        break;
                    case LUA_TNUMBER:
                        printf("\t\t\t%d: %g", i, lua_tonumber(L, i));
                        break;
                    default:
                        printf("\t\t\t%d: %s", i, lua_typename(L, t));
                        break;
                }
                i--;
                printf("\n");
            }
//        LOG(INFO) << "--------------- Stack Dump Finished ---------------";
            printf("\n");
        }
    }
}