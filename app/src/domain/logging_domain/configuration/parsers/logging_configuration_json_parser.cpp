#include "logging_configuration_json_parser.h"

namespace eerie_leap::domain::logging_domain::configuration::parsers {

ext_unique_ptr<JsonLoggingConfig> LoggingConfigurationJsonParser::Serialize(const LoggingConfiguration& configuration) {
    auto config = make_unique_ext<JsonLoggingConfig>();

    config->logging_interval_ms = configuration.logging_interval_ms;
    config->max_log_size_mb = configuration.max_log_size_mb;

    for(auto& [sensor_id, sensor_configuration] : configuration.sensor_configurations) {
        JsonSensorLoggingConfig sensor_logging_config {
            .sensor_id_hash = sensor_id,
            .is_enabled = sensor_configuration.is_enabled,
            .log_raw_value = sensor_configuration.log_raw_value,
            .log_only_new_data = sensor_configuration.log_only_new_data
        };

        config->sensor_configs.push_back(sensor_logging_config);
    }

    return config;
}

pmr_unique_ptr<LoggingConfiguration> LoggingConfigurationJsonParser::Deserialize(std::pmr::memory_resource* mr, const JsonLoggingConfig& config) {
    auto configuration = make_unique_pmr<LoggingConfiguration>(mr);
    configuration->logging_interval_ms = config.logging_interval_ms;
    configuration->max_log_size_mb = config.max_log_size_mb;

    for(auto& sensor_logging_config : config.sensor_configs) {
        SensorLoggingConfiguration sensor_logging_configuration {
            .is_enabled = sensor_logging_config.is_enabled,
            .log_raw_value = sensor_logging_config.log_raw_value,
            .log_only_new_data = sensor_logging_config.log_only_new_data
        };

        configuration->sensor_configurations.emplace(sensor_logging_config.sensor_id_hash, sensor_logging_configuration);
    }

    return configuration;
}

} // eerie_leap::domain::logging_domain::configuration::parsers
