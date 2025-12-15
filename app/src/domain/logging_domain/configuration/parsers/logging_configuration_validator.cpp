#include <stdexcept>

#include "logging_configuration_validator.h"

namespace eerie_leap::domain::logging_domain::configuration::parsers {

static void InvalidLoggingConfiguration(std::string_view message) {
    throw std::invalid_argument(
        "Invalid Logging configuration. "
        + std::string(message));
}

void LoggingConfigurationValidator::Validate(const LoggingConfiguration& configuration) {

    ValidateLoggingIntervalMs(configuration);
    ValidateMaxLogSizeMb(configuration);
    ValidateSensorConfigurations(configuration);
}

void LoggingConfigurationValidator::ValidateLoggingIntervalMs(const LoggingConfiguration& configuration) {
    for(const auto& logging_interval_ms : {configuration.logging_interval_ms}) {
        if(logging_interval_ms < 10) {
            InvalidLoggingConfiguration("Logging interval must be greater or equal than 10 ms.");
        }
    }
}

void LoggingConfigurationValidator::ValidateMaxLogSizeMb(const LoggingConfiguration& configuration) {
    for(const auto& max_log_size_mb : {configuration.max_log_size_mb}) {
        if(max_log_size_mb > 1024) {
            InvalidLoggingConfiguration("Max log size must be less or equal than 1024 MB.");
        }
    }
}

void LoggingConfigurationValidator::ValidateSensorConfigurations(const LoggingConfiguration& configuration) {
    // Nothing to validate
}

} // namespace eerie_leap::domain::logging_domain::configuration::parsers
