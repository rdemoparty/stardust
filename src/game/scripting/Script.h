#pragma once

#include <luaconf.h>
#include <lua.hpp>

namespace Acidrain {

    class Script {
    public:
        Script(const char* const scriptUri);
        ~Script();

    private:
        lua_State* L;
    };
}