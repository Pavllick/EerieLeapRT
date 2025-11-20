#include "script_processor.h"

namespace eerie_leap::domain::sensor_domain::processors {

ScriptProcessor::ScriptProcessor(const std::string& function_name)
    : function_name_(function_name) {}

void ScriptProcessor::ProcessReading(std::shared_ptr<SensorReading> reading) {
    auto lua_script = reading->sensor->configuration.lua_script;
    if(lua_script == nullptr)
        return;

    auto* state = lua_script->GetState();
    if(state == nullptr)
        return;

    lua_getglobal(state, function_name_.c_str());

    if(!lua_isfunction(state, -1)) {
        lua_pop(state, 1);
        return;
    }

    lua_pushstring(state, reading->sensor->id.c_str());

    if(lua_pcall(state, 1, 0, 0) != LUA_OK)
        lua_pop(state, 1);
}

} // namespace eerie_leap::domain::sensor_domain::processors
