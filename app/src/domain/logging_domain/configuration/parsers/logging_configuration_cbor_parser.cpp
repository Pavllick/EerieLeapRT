#include <algorithm>

#include "logging_configuration_validator.h"
#include "logging_configuration_cbor_parser.h"

namespace eerie_leap::domain::logging_domain::configuration::parsers {

pmr_unique_ptr<CborLoggingConfig> LoggingConfigurationCborParser::Serialize(const LoggingConfiguration& configuration) {
    LoggingConfigurationValidator::Validate(configuration);

    auto logging_config = make_unique_pmr<CborLoggingConfig>(Mrm::GetExtPmr());

    logging_config->logging_interval_ms = configuration.logging_interval_ms;
    logging_config->max_log_size_mb = configuration.max_log_size_mb;

    for(auto& [sensor_id, sensor_configuration] : configuration.sensor_configurations) {
        CborSensorLoggingConfig sensor_logging_config {
            .sensor_id_hash = sensor_id,
            .is_enabled = sensor_configuration.is_enabled,
            .log_raw_value = sensor_configuration.log_raw_value,
            .log_only_new_data = sensor_configuration.log_only_new_data
        };

        logging_config->CborSensorLoggingConfig_m.push_back(std::move(sensor_logging_config));
    }

    return logging_config;
}

pmr_unique_ptr<LoggingConfiguration> LoggingConfigurationCborParser::Deserialize(std::pmr::memory_resource* mr, const CborLoggingConfig& config) {
    auto configuration = make_unique_pmr<LoggingConfiguration>(mr);
    configuration->logging_interval_ms = config.logging_interval_ms;
    configuration->max_log_size_mb = config.max_log_size_mb;

    for(const auto& sensor_logging_config : config.CborSensorLoggingConfig_m) {
        SensorLoggingConfiguration sensor_logging_configuration {
            .is_enabled = sensor_logging_config.is_enabled,
            .log_raw_value = sensor_logging_config.log_raw_value,
            .log_only_new_data = sensor_logging_config.log_only_new_data
        };

        configuration->sensor_configurations.emplace(sensor_logging_config.sensor_id_hash, std::move(sensor_logging_configuration));
    }

    LoggingConfigurationValidator::Validate(*configuration);

    return configuration;
}

} // namespace eerie_leap::domain::logging_domain::configuration::parsers
