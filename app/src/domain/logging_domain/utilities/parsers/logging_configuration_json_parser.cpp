#include "logging_configuration_json_parser.h"

namespace eerie_leap::domain::logging_domain::utilities::parsers {

ext_unique_ptr<JsonLoggingConfig> LoggingConfigurationJsonParser::Serialize(const LoggingConfiguration& configuration) {
    auto config = make_unique_ext<JsonLoggingConfig>();
    memset(config.get(), 0, sizeof(JsonLoggingConfig));

    config->logging_interval_ms = configuration.logging_interval_ms;
    config->max_log_size_mb = configuration.max_log_size_mb;

    config->sensor_configurations_len = 0;

    int i = 0;
    for(auto& [sensor_id, sensor_configuration] : configuration.sensor_configurations) {
        JsonSensorLoggingConfig sensor_logging_configuration_json {
            .sensor_id_hash = sensor_id,
            .is_enabled = sensor_configuration.is_enabled,
            .log_raw_value = sensor_configuration.log_raw_value,
            .log_only_new_data = sensor_configuration.log_only_new_data
        };

        config->sensor_configurations[i] = sensor_logging_configuration_json;
        config->sensor_configurations_len++;
        i++;
    }

    return config;
}

LoggingConfiguration LoggingConfigurationJsonParser::Deserialize(const JsonLoggingConfig& config) {
    LoggingConfiguration configuration {
        .logging_interval_ms = config.logging_interval_ms,
        .max_log_size_mb = config.max_log_size_mb
    };

    for(size_t i = 0; i < config.sensor_configurations_len; i++) {
        SensorLoggingConfiguration sensor_logging_configuration {
            .is_enabled = config.sensor_configurations[i].is_enabled,
            .log_raw_value = config.sensor_configurations[i].log_raw_value,
            .log_only_new_data = config.sensor_configurations[i].log_only_new_data
        };
        configuration.sensor_configurations[config.sensor_configurations[i].sensor_id_hash] = sensor_logging_configuration;
    }

    return configuration;
}

} // eerie_leap::domain::logging_domain::utilities::parsers
