#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

namespace eerie_leap::subsys::lua_script {

class LuaScript {
private:
    lua_State *state_;

    static int sleep_ms_func(lua_State *state);
    static int print_func(lua_State *state);

    static const std::vector<luaL_Reg> static_global_functions_;
    std::unordered_map<std::string, lua_CFunction> global_functions_;

public:
    LuaScript();
    ~LuaScript();

    lua_State* GetState() { return state_; }
    void Run(std::string_view script);
    void RegisterGlobalFunction(const std::string& name, lua_CFunction func, void* object = nullptr);
};

} // namespace eerie_leap::subsys::lua_script
