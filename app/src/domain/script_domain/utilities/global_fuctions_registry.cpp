#include "global_fuctions_registry.h"

namespace eerie_leap::domain::script_domain::utilities {

using namespace eerie_leap::subsys::lua_script;
using namespace eerie_leap::domain::sensor_domain::utilities;

void GlobalFunctionsRegistry::RegisterGetSensorValue(LuaScript& lua_script, SensorReadingsFrame& sensor_readings_frame) {
    lua_script.RegisterGlobalFunction("get_sensor_value", &GlobalFunctionsRegistry::LuaGetSensorValue, &sensor_readings_frame);
}

void GlobalFunctionsRegistry::RegisterUpdateSensorValue(LuaScript& lua_script, SensorReadingsFrame& sensor_readings_frame) {
    lua_script.RegisterGlobalFunction("update_sensor_value", &GlobalFunctionsRegistry::LuaUpdateSensorValue, &sensor_readings_frame);
}

int GlobalFunctionsRegistry::LuaGetSensorValue(lua_State* state) {
    if(lua_gettop(state) != 1)
        return luaL_error(state, "Expected 1 argument");

    auto* sensor_readings_frame =
        static_cast<SensorReadingsFrame*>(lua_touserdata(state, lua_upvalueindex(1)));

    const char* sensor_id = luaL_checkstring(state, 1);

    if(!sensor_readings_frame->HasReading(sensor_id) || !sensor_readings_frame->GetReading(sensor_id)->value.has_value()) {
        lua_pushnil(state);
        return 1;
    }

    float result = sensor_readings_frame->GetReading(sensor_id)->value.value();
    lua_pushnumber(state, result);

    return 1;
}

int GlobalFunctionsRegistry::LuaUpdateSensorValue(lua_State* state) {
    if(lua_gettop(state) != 2)
        return luaL_error(state, "Expected 2 arguments");

    auto* sensor_readings_frame =
        static_cast<SensorReadingsFrame*>(lua_touserdata(state, lua_upvalueindex(1)));

    const char* sensor_id = luaL_checkstring(state, 1);
    float value = luaL_checknumber(state, 2);

    if(!sensor_readings_frame->HasReading(sensor_id)) {
        lua_pushboolean(state, false);
        return 1;
    }

    auto reading = sensor_readings_frame->GetReading(sensor_id);
    reading->value = value;

    sensor_readings_frame->AddOrUpdateReading(reading);

    lua_pushboolean(state, true);

    return 1;
}

}
