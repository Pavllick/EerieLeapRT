#pragma once

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "configuration/json/configs/json_system_config.h"
#include "domain/system_domain/models/system_configuration.h"

namespace eerie_leap::domain::system_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::domain::system_domain::models;

class SystemConfigurationJsonParser {
public:
    SystemConfigurationJsonParser() = default;

    ext_unique_ptr<JsonSystemConfig> Serialize(const SystemConfiguration& configuration);
    SystemConfiguration Deserialize(
        const JsonSystemConfig& config,
        uint64_t device_id,
        uint32_t hw_version,
        uint32_t sw_version,
        uint32_t build_number);
};

} // namespace eerie_leap::domain::system_domain::configuration::parsers
