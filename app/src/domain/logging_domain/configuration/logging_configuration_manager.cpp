#include "logging_configuration_manager.h"

namespace eerie_leap::domain::logging_domain::configuration {

LOG_MODULE_REGISTER(logging_config_ctrl_logger);

LoggingConfigurationManager::LoggingConfigurationManager() :
    logging_configuration_(nullptr) {}

bool LoggingConfigurationManager::Update(std::shared_ptr<LoggingConfiguration> logging_configuration) {
    logging_configuration_ = logging_configuration;

    return true;
}

std::shared_ptr<LoggingConfiguration> LoggingConfigurationManager::Get(bool force_load) {
    if (logging_configuration_ != nullptr && !force_load) {
        return logging_configuration_;
    }

    return logging_configuration_;
}

} // namespace eerie_leap::domain::logging_domain::configuration
