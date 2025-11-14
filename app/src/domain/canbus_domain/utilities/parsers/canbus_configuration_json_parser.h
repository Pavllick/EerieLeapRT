#pragma once

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "configuration/json/configs/json_canbus_config.h"
#include "domain/canbus_domain/models/canbus_configuration.h"

namespace eerie_leap::domain::canbus_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::domain::canbus_domain::models;

class CanbusConfigurationJsonParser {
public:
    CanbusConfigurationJsonParser() = default;

    ext_unique_ptr<JsonCanbusConfig> Serialize(const CanbusConfiguration& configuration);
    CanbusConfiguration Deserialize(const JsonCanbusConfig& config);
};

} // namespace eerie_leap::domain::canbus_domain::utilities::parsers
