#pragma once

#include <string_view>
#include <vector>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

namespace eerie_leap::subsys::lua {

class LuaWrapper {
private:
    lua_State *state_;

    static int sleep_ms_func(lua_State *state);
    static int print_func(lua_State *state);

    static const std::vector<luaL_Reg> static_functions_;

public:
    LuaWrapper();
    ~LuaWrapper();

    void RunScript(std::string_view script);
};

} // namespace eerie_leap::subsys::lua
