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

        void dumpStack(lua_State* L) {
            int i;
            int top = lua_gettop(L);

            printf("total in stack %d\n", top);

            for (i = 1; i <= top; i++) {
                int t = lua_type(L, i);
                switch (t) {
                    case LUA_TSTRING:
                        printf("string: '%s'\n", lua_tostring(L, i));
                        break;
                    case LUA_TBOOLEAN:
                        printf("boolean %s\n", lua_toboolean(L, i) ? "true" : "false");
                        break;
                    case LUA_TNUMBER:
                        printf("number: %g\n", lua_tonumber(L, i));
                        break;
                    default:
                        printf("%s\n", lua_typename(L, t));
                        break;
                }
                printf("  ");
            }
            printf("\n");
        }
    }
}