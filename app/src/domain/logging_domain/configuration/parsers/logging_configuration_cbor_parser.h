#pragma once

#include "utilities/memory/memory_resource_manager.h"
#include "configuration/cbor/cbor_logging_config/cbor_logging_config.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::logging_domain::models;

class LoggingConfigurationCborParser {
public:
    LoggingConfigurationCborParser() = default;

    ext_unique_ptr<CborLoggingConfig> Serialize(const LoggingConfiguration& logging_configuration);
    pmr_unique_ptr<LoggingConfiguration> Deserialize(const CborLoggingConfig& logging_config);
};

} // namespace eerie_leap::domain::logging_domain::configuration::parsers
