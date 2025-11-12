#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_logging_config/cbor_logging_config.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::logging_domain::models;

class LoggingConfigurationCborParser {
public:
    LoggingConfigurationCborParser() = default;

    ext_unique_ptr<CborLoggingConfig> Serialize(const LoggingConfiguration& logging_configuration);
    LoggingConfiguration Deserialize(const CborLoggingConfig& logging_config);
};

} // namespace eerie_leap::domain::logging_domain::utilities::parsers
