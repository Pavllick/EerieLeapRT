#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "lua_wrapper.h"

namespace eerie_leap::subsys::lua {

LOG_MODULE_REGISTER(lua);

LuaWrapper::LuaWrapper() : state_(luaL_newstate()) {
	lua_pushglobaltable(state_);
    luaL_setfuncs(state_, static_functions_.data(), 0);
    lua_pop(state_, 1);
}

LuaWrapper::~LuaWrapper() {
	lua_close(state_);
}

void LuaWrapper::RunScript(std::string_view script) {
	if(luaL_dostring(state_, script.data()) != LUA_OK) {
		LOG_ERR("Error running script: %s", lua_tostring(state_, -1));
		lua_pop(state_, 1);
	}
}

int LuaWrapper::sleep_ms_func(lua_State *state) {
	int n = lua_gettop(state);
	if(n != 1) {
		return luaL_error(state, "expected 1 arguments, got %d", n);
	}

	int ms = luaL_checkinteger(state, 1);

	k_msleep(ms);

	return 0;
}

int LuaWrapper::print_func(lua_State *state) {
	int n = lua_gettop(state);
	if(n != 1) {
		return luaL_error(state, "expected 1 arguments, got %d", n);
	}

	const char *message = luaL_checkstring(state, 1);

	printk("%s\n", message);

	return 0;
}

const std::vector<luaL_Reg> LuaWrapper::static_functions_ = {
	{"sleep_ms", sleep_ms_func},
	{"print", print_func},
	{NULL, NULL} // Sentinel value to mark the end of the array
};

} // namespace eerie_leap::subsys::lua
