#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/system_config/system_config.h"
#include "configuration/services/configuration_service.h"
#include "domain/logging_domain/models/logging_configuration.h"

namespace eerie_leap::domain::logging_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::logging_domain::models;

class LoggingConfigurationManager {
private:
    std::shared_ptr<LoggingConfiguration> logging_configuration_;

public:
    LoggingConfigurationManager();

    bool Update(std::shared_ptr<LoggingConfiguration> logging_configuration);
    std::shared_ptr<LoggingConfiguration> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::logging_domain::configuration
