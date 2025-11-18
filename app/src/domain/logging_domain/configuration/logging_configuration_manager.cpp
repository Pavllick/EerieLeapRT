#include <zephyr/logging/log.h>

#include "logging_configuration_manager.h"
#include "domain/logging_domain/utilities/parsers/logging_configuration_cbor_parser.h"

namespace eerie_leap::domain::logging_domain::configuration {

LOG_MODULE_REGISTER(logging_config_ctrl_logger);

LoggingConfigurationManager::LoggingConfigurationManager(
    ext_unique_ptr<CborConfigurationService<CborLoggingConfig>> cbor_configuration_service,
    ext_unique_ptr<JsonConfigurationService<JsonLoggingConfig>> json_configuration_service)
        : cbor_configuration_service_(std::move(cbor_configuration_service)),
        json_configuration_service_(std::move(json_configuration_service)),
        configuration_(nullptr) {

    cbor_parser_ = std::make_unique<LoggingConfigurationCborParser>();
    json_parser_ = std::make_unique<LoggingConfigurationJsonParser>();
    std::shared_ptr<LoggingConfiguration> configuration = nullptr;

    try {
        configuration = Get(true);
    } catch(const std::exception& e) {}

    if(configuration == nullptr) {
        if(!CreateDefaultConfiguration()) {
            LOG_ERR("Failed to create default Logging configuration.");
            return;
        }

        LOG_INF("Default Logging configuration loaded successfully.");
    }

    LOG_INF("Logging Configuration Manager initialized successfully.");

    ApplyJsonConfiguration();
}

bool LoggingConfigurationManager::ApplyJsonConfiguration() {
    if(!json_configuration_service_->IsAvailable())
        return false;

    auto json_config_loaded = json_configuration_service_->Load();
    if(json_config_loaded.has_value()) {
        if(json_config_loaded->checksum == json_config_checksum_)
            return true;

        try {
            auto configuration = json_parser_->Deserialize(*json_config_loaded->config);

            if(!Update(configuration))
                return false;
        } catch(const std::exception& e) {
            LOG_ERR("Failed to deserialize JSON configuration. %s", e.what());
            return false;
        }

        LOG_INF("JSON configuration loaded successfully.");

        return true;
    }

    return Update(*configuration_);
}

bool LoggingConfigurationManager::Update(const LoggingConfiguration& configuration) {
    try {
        if(json_configuration_service_->IsAvailable()) {
            auto json_config = json_parser_->Serialize(configuration);
            json_configuration_service_->Save(json_config.get());

            auto json_config_loaded = json_configuration_service_->Load();
            if(!json_config_loaded.has_value()) {
                LOG_ERR("Failed to load newly updated JSON configuration.");
                return false;
            }

            LOG_INF("JSON configuration updated successfully.");

            json_config_checksum_ = json_config_loaded->checksum;
        }

        auto cbor_config = cbor_parser_->Serialize(configuration);
        cbor_config->json_config_checksum = json_config_checksum_;

        if(!cbor_configuration_service_->Save(cbor_config.get()))
            return false;
    } catch(const std::exception& e) {
        LOG_ERR("Failed to update Logging configuration. %s", e.what());
        return false;
    }

    Get(true);

    return true;
}

std::shared_ptr<LoggingConfiguration> LoggingConfigurationManager::Get(bool force_load) {
    if (configuration_ != nullptr && !force_load) {
        return configuration_;
    }

    auto cbor_config_data = cbor_configuration_service_->Load();
    if(!cbor_config_data.has_value())
        return nullptr;

    auto cbor_config = std::move(cbor_config_data.value().config);

    auto configuration = cbor_parser_->Deserialize(*cbor_config);
    configuration_ = make_shared_ext<LoggingConfiguration>(configuration);

    json_config_checksum_ = cbor_config->json_config_checksum;

    return configuration_;
}

bool LoggingConfigurationManager::CreateDefaultConfiguration() {
    auto configuration = make_unique_ext<LoggingConfiguration>();

    return Update(*configuration);
}

} // namespace eerie_leap::domain::logging_domain::configuration
