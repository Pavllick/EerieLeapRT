#pragma once

#include <zephyr/data/json.h>

#include "utilities/memory/memory_resource_manager.h"
#include "configuration/json/configs/json_logging_config.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::domain::logging_domain::models;

class LoggingConfigurationJsonParser {
public:
    LoggingConfigurationJsonParser() = default;

    ext_unique_ptr<JsonLoggingConfig> Serialize(const LoggingConfiguration& configuration);
    pmr_unique_ptr<LoggingConfiguration> Deserialize(std::pmr::memory_resource* mr, const JsonLoggingConfig& config);
};

} // namespace eerie_leap::domain::logging_domain::configuration::parsers
