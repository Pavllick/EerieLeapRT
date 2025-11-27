#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <span>
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

    static const luaL_Reg lua_std_libs_[];
    static const luaL_Reg static_global_functions_[];
    std::unordered_map<std::string, lua_CFunction> global_functions_;

    LuaScript(lua_State *state);

    void OpenLuaStdLibs(lua_State *L);
    static void* ExtMemoryAllocator(void* ud, void* ptr, size_t osize, size_t nsize);

public:
    ~LuaScript();

    LuaScript(const LuaScript&) = delete;
    LuaScript& operator=(const LuaScript&) = delete;
    LuaScript(LuaScript&& other) noexcept : state_(other.state_) {
        other.state_ = nullptr;
    }

    // NOTE: Create() will use the default memory allocator
    //       CreateExt() will use the external memory allocator, if available
    //       or the default memory allocator if not available
    static LuaScript Create();
    static LuaScript CreateExt();

    lua_State* GetState() { return state_; }
    void Load(const std::span<const uint8_t>& script);
    void RegisterGlobalFunction(const std::string& name, lua_CFunction func, void* object = nullptr);

    size_t GetMemoryUsedKb() const;
    void LogMemoryUsage() const;
};

} // namespace eerie_leap::subsys::lua_script
