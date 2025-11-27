#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "utilities/memory/heap_allocator.h"

#include "lua_script.h"

namespace eerie_leap::subsys::lua_script {

using namespace eerie_leap::utilities::memory;

LOG_MODULE_REGISTER(lua);

LuaScript::LuaScript(lua_State *state) : state_(state) {
	if(state_ == nullptr) {
		LOG_ERR("Failed to create Lua state.");
		return;
	}

	OpenLuaStdLibs(state_);
	lua_pushglobaltable(state_);
    luaL_setfuncs(state_, static_global_functions_, 0);
    lua_pop(state_, 1);
}

LuaScript::~LuaScript() {
	if(state_ != nullptr)
		lua_close(state_);
}

LuaScript LuaScript::Create() {
    return {luaL_newstate()};
}

LuaScript LuaScript::CreateExt() {
    return {lua_newstate(ExtMemoryAllocator, nullptr)};
}

void LuaScript::Load(const std::span<const uint8_t>& script) {
	if(state_ == nullptr) {
		LOG_ERR("Cannot load script: Lua state is null");
		return;
	}

	if(luaL_loadbuffer(state_, reinterpret_cast<const char*>(script.data()),
						script.size(), "script") != LUA_OK) {
		LOG_ERR("Error loading script: %s", lua_tostring(state_, -1));
		lua_pop(state_, 1);
		return;
	}

	if(lua_pcall(state_, 0, 0, 0) != LUA_OK) {
		LOG_ERR("Error executing script: %s", lua_tostring(state_, -1));
		lua_pop(state_, 1);
		return;
	}
}

void LuaScript::RegisterGlobalFunction(const std::string& name, lua_CFunction func, void* object) {
	global_functions_.insert({name, func});

	if(object != nullptr)
		lua_pushlightuserdata(state_, object);
	lua_pushcclosure(state_, func, 1);
	lua_setglobal(state_, name.c_str());
}

size_t LuaScript::GetMemoryUsedKb() const {
	if(state_ == nullptr)
		return 0;

	return lua_gc(state_, LUA_GCCOUNT, 0) * 1024 +
		lua_gc(state_, LUA_GCCOUNTB, 0);
}

void LuaScript::LogMemoryUsage() const {
	LOG_INF("Lua state memory usage: %zu bytes", GetMemoryUsedKb());
}

void* LuaScript::ExtMemoryAllocator(void* ud, void* ptr, size_t osize, size_t nsize) {
	HeapAllocator<uint8_t> allocator;

	if(nsize == 0) {
		// Free request
		if(ptr != nullptr)
			allocator.deallocate(static_cast<uint8_t*>(ptr), osize);

		return nullptr;
	}

	void* new_ptr = nullptr;

	if(ptr == nullptr) {
		new_ptr = allocator.allocate(nsize);
	} else {
		new_ptr = allocator.reallocate(static_cast<uint8_t*>(ptr), osize, nsize);
	}

	if(new_ptr == nullptr && nsize > 0)
		LOG_ERR("Lua allocation failed: %zu bytes", nsize);

	return new_ptr;
}

int LuaScript::sleep_ms_func(lua_State *state) {
	int n = lua_gettop(state);
	if(n != 1)
		return luaL_error(state, "expected 1 arguments, got %d", n);

	int ms = luaL_checkinteger(state, 1);

	k_msleep(ms);

	return 0;
}

int LuaScript::print_func(lua_State *state) {
	int n = lua_gettop(state);
	if(n != 1)
		return luaL_error(state, "expected 1 arguments, got %d", n);

	const char *message = luaL_checkstring(state, 1);

	printk("%s\n", message);

	return 0;
}

void LuaScript::OpenLuaStdLibs(lua_State *L) {
	const luaL_Reg *lib;
	/* "require" functions from 'loadedlibs' and set results to global table */
	for(lib = lua_std_libs_; lib->func; lib++) {
		luaL_requiref(L, lib->name, lib->func, 1);
		lua_pop(L, 1);  /* remove lib */
	}
}

/*
** these libs are loaded by lua.c and are readily available to any Lua
** program
*/
const luaL_Reg LuaScript::lua_std_libs_[] = {
	// {LUA_GNAME, luaopen_base},
	// {LUA_LOADLIBNAME, luaopen_package},
	// {LUA_COLIBNAME, luaopen_coroutine},
	{LUA_TABLIBNAME, luaopen_table},
	// {LUA_IOLIBNAME, luaopen_io},
	// {LUA_OSLIBNAME, luaopen_os},
	{LUA_STRLIBNAME, luaopen_string},
	{LUA_MATHLIBNAME, luaopen_math},
	{LUA_UTF8LIBNAME, luaopen_utf8},
	// {LUA_DBLIBNAME, luaopen_debug},
	{NULL, NULL} // Array end marker
};

const luaL_Reg LuaScript::static_global_functions_[] = {
	{"sleep_ms", sleep_ms_func},
	{"print", print_func},
	{NULL, NULL} // Array end marker
};

} // namespace eerie_leap::subsys::lua_script
