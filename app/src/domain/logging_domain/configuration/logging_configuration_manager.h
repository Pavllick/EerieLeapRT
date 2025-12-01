#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_logging_config/cbor_logging_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "configuration/services/json_configuration_service.h"
#include "domain/logging_domain/configuration/parsers/logging_configuration_cbor_parser.h"
#include "domain/logging_domain/configuration/parsers/logging_configuration_json_parser.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::logging_domain::configuration::parsers;
using namespace eerie_leap::domain::logging_domain::models;

class LoggingConfigurationManager {
private:
    std::unique_ptr<CborConfigurationService<CborLoggingConfig>> cbor_configuration_service_;
    std::unique_ptr<JsonConfigurationService<JsonLoggingConfig>> json_configuration_service_;

    ext_unique_ptr<LoggingConfigurationCborParser> cbor_parser_;
    ext_unique_ptr<LoggingConfigurationJsonParser> json_parser_;

    std::shared_ptr<LoggingConfiguration> configuration_;

    uint32_t json_config_checksum_;

    bool ApplyJsonConfiguration();
    bool CreateDefaultConfiguration();

public:
    LoggingConfigurationManager(
        std::unique_ptr<CborConfigurationService<CborLoggingConfig>> cbor_configuration_service,
        std::unique_ptr<JsonConfigurationService<JsonLoggingConfig>> json_configuration_service);

    bool Update(const LoggingConfiguration& configuration, bool internal_only = false);
    std::shared_ptr<LoggingConfiguration> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::logging_domain::configuration
