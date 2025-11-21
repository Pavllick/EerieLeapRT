#pragma once

#include <cstdint>
#include <string>

#include "utilities/memory/heap_allocator.h"
#include "subsys/lua_script/lua_script.h"

namespace eerie_leap::domain::canbus_domain::models {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::lua_script;

struct CanMessageConfiguration {
    uint32_t frame_id;
    uint32_t send_interval_ms;
    std::string script_path;

    std::shared_ptr<LuaScript> lua_script = nullptr;
};

} // namespace eerie_leap::domain::canbus_domain::models
