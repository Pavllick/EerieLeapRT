#pragma once

#include <span>

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "configuration/json/configs/json_logging_config.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::domain::logging_domain::models;

class LoggingConfigurationJsonParser {
public:
    LoggingConfigurationJsonParser() = default;

    ext_unique_ptr<JsonLoggingConfig> Serialize(const LoggingConfiguration& configuration);
    LoggingConfiguration Deserialize(const JsonLoggingConfig& config);
};

} // namespace eerie_leap::domain::logging_domain::utilities::parsers
