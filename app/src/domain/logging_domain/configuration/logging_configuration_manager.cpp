#include "logging_configuration_manager.h"

namespace eerie_leap::domain::logging_domain::configuration {

LOG_MODULE_REGISTER(logging_config_ctrl_logger);

LoggingConfigurationManager::LoggingConfigurationManager(ext_unique_ptr<ConfigurationService<LoggingConfig>> logging_configuration_service) :
    logging_configuration_service_(std::move(logging_configuration_service)),
    logging_config_(nullptr),
    logging_configuration_(nullptr) {

    if(Get(true) == nullptr) {
        if(!CreateDefaultConfiguration()) {
            LOG_ERR("Failed to create default Logging configuration.");
            return;
        }
    }

    LOG_INF("Logging Configuration Manager initialized successfully.");
}

bool LoggingConfigurationManager::Update(std::shared_ptr<LoggingConfiguration> logging_configuration) {
    LoggingConfig logging_config;
    memset(&logging_config, 0, sizeof(LoggingConfig));

    logging_config.logging_interval_ms = logging_configuration->logging_interval_ms;
    logging_config.max_log_size_mb = logging_configuration->max_log_size_mb;

    int i = 0;
    for(auto& [sensor_id, sensor_configuration] : logging_configuration->sensor_configurations) {
        SensorLoggingConfig sensor_config {
            .is_enabled = sensor_configuration.is_enabled,
            .log_raw_value = sensor_configuration.log_raw_value,
            .log_only_new_data = sensor_configuration.log_only_new_data
        };

        logging_config.sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m_key = sensor_id;
        logging_config.sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m = sensor_config;

        logging_config.sensor_configurations.SensorLoggingConfig_m_count++;
        i++;
    }

    if(!logging_configuration_service_->Save(&logging_config))
        return false;

    Get(true);

    return true;
}

std::shared_ptr<LoggingConfiguration> LoggingConfigurationManager::Get(bool force_load) {
    if (logging_configuration_ != nullptr && !force_load) {
        return logging_configuration_;
    }

    auto logging_config = logging_configuration_service_->Load();
    if(!logging_config.has_value())
        return nullptr;

    logging_config_raw_ = std::move(logging_config.value().config_raw);
    logging_config_ = std::move(logging_config.value().config);

    LoggingConfiguration logging_configuration {
        .logging_interval_ms = logging_config_->logging_interval_ms,
        .max_log_size_mb = logging_config_->max_log_size_mb
    };

    for(size_t i = 0; i < logging_config_->sensor_configurations.SensorLoggingConfig_m_count; i++) {
        SensorLoggingConfiguration sensor_configuration {
            .is_enabled = logging_config_->sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m.is_enabled,
            .log_raw_value = logging_config_->sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m.log_raw_value,
            .log_only_new_data = logging_config_->sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m.log_only_new_data
        };
        logging_configuration.sensor_configurations[logging_config_->sensor_configurations.SensorLoggingConfig_m[i].SensorLoggingConfig_m_key] = sensor_configuration;
    }

    logging_configuration_ = make_shared_ext<LoggingConfiguration>(logging_configuration);

    return logging_configuration_;
}

bool LoggingConfigurationManager::CreateDefaultConfiguration() {
    auto logging_configuration = make_shared_ext<LoggingConfiguration>();

    return Update(logging_configuration);
}

} // namespace eerie_leap::domain::logging_domain::configuration
