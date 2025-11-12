#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_logging_config/cbor_logging_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "configuration/services/json_configuration_service.h"
#include "domain/logging_domain/utilities/parsers/logging_configuration_cbor_parser.h"
#include "domain/logging_domain/utilities/parsers/logging_configuration_json_parser.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::logging_domain::utilities::parsers;
using namespace eerie_leap::domain::logging_domain::models;

class LoggingConfigurationManager {
private:
    ext_unique_ptr<CborConfigurationService<CborLoggingConfig>> cbor_configuration_service_;
    ext_unique_ptr<JsonConfigurationService<JsonLoggingConfig>> json_configuration_service_;

    std::unique_ptr<LoggingConfigurationCborParser> cbor_parser_;
    std::unique_ptr<LoggingConfigurationJsonParser> json_parser_;

    ext_unique_ptr<ExtVector> cbor_config_raw_;
    ext_unique_ptr<CborLoggingConfig> cbor_config_;
    std::shared_ptr<LoggingConfiguration> configuration_;

    uint32_t json_config_checksum_;

    bool ApplyJsonConfiguration();
    bool CreateDefaultConfiguration();

public:
    LoggingConfigurationManager(
        ext_unique_ptr<CborConfigurationService<CborLoggingConfig>> cbor_configuration_service,
        ext_unique_ptr<JsonConfigurationService<JsonLoggingConfig>> json_configuration_service);

    bool Update(const LoggingConfiguration& configuration);
    std::shared_ptr<LoggingConfiguration> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::logging_domain::configuration
