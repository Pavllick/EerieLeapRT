#include <algorithm>

#include "logging_configuration_cbor_parser.h"

namespace eerie_leap::domain::logging_domain::utilities::parsers {

ext_unique_ptr<CborLoggingConfig> LoggingConfigurationCborParser::Serialize(const LoggingConfiguration& logging_configuration) {
    auto logging_config = make_unique_ext<CborLoggingConfig>();
    memset(logging_config.get(), 0, sizeof(CborLoggingConfig));

    logging_config->logging_interval_ms = logging_configuration.logging_interval_ms;
    logging_config->max_log_size_mb = logging_configuration.max_log_size_mb;

    logging_config->CborSensorLoggingConfig_m_count = 0;

    int i = 0;
    for(auto& [sensor_id, sensor_configuration] : logging_configuration.sensor_configurations) {
        CborSensorLoggingConfig sensor_logging_config {
            .sensor_id_hash = sensor_id,
            .is_enabled = sensor_configuration.is_enabled,
            .log_raw_value = sensor_configuration.log_raw_value,
            .log_only_new_data = sensor_configuration.log_only_new_data
        };

        logging_config->CborSensorLoggingConfig_m[i] = sensor_logging_config;
        logging_config->CborSensorLoggingConfig_m_count++;
        i++;
    }

    return logging_config;
}

LoggingConfiguration LoggingConfigurationCborParser::Deserialize(const CborLoggingConfig& logging_config) {
    LoggingConfiguration logging_configuration {
        .logging_interval_ms = logging_config.logging_interval_ms,
        .max_log_size_mb = logging_config.max_log_size_mb
    };

    for(size_t i = 0; i < logging_config.CborSensorLoggingConfig_m_count; i++) {
        SensorLoggingConfiguration sensor_logging_configuration {
            .is_enabled = logging_config.CborSensorLoggingConfig_m[i].is_enabled,
            .log_raw_value = logging_config.CborSensorLoggingConfig_m[i].log_raw_value,
            .log_only_new_data = logging_config.CborSensorLoggingConfig_m[i].log_only_new_data
        };
        logging_configuration.sensor_configurations[logging_config.CborSensorLoggingConfig_m[i].sensor_id_hash] = sensor_logging_configuration;
    }

    return logging_configuration;
}

} // namespace eerie_leap::domain::logging_domain::utilities::parsers
