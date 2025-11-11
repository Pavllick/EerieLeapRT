#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/logging_config/logging_config.h"
#include "configuration/services/configuration_service.h"
#include "domain/logging_domain/utilities/parsers/logging_configuration_cbor_parser.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::logging_domain::utilities::parsers;
using namespace eerie_leap::domain::logging_domain::models;

class LoggingConfigurationManager {
private:
    ext_unique_ptr<ConfigurationService<LoggingConfig>> logging_configuration_service_;
    ext_unique_ptr<ExtVector> logging_config_raw_;
    ext_unique_ptr<LoggingConfig> logging_config_;
    std::shared_ptr<LoggingConfiguration> logging_configuration_;
    std::unique_ptr<LoggingConfigurationCborParser> logging_configuration_cbor_parser_;

    bool CreateDefaultConfiguration();

public:
    explicit LoggingConfigurationManager(ext_unique_ptr<ConfigurationService<LoggingConfig>> logging_configuration_service);

    bool Update(std::shared_ptr<LoggingConfiguration> logging_configuration);
    std::shared_ptr<LoggingConfiguration> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::logging_domain::configuration
