#include "logging_configuration_manager.h"
#include "domain/logging_domain/utilities/parsers/logging_configuration_cbor_parser.h"

namespace eerie_leap::domain::logging_domain::configuration {

LOG_MODULE_REGISTER(logging_config_ctrl_logger);

LoggingConfigurationManager::LoggingConfigurationManager(ext_unique_ptr<CborConfigurationService<CborLoggingConfig>> cbor_configuration_service) :
    cbor_configuration_service_(std::move(cbor_configuration_service)),
    config_(nullptr),
    configuration_(nullptr) {

    cbor_parser_ = std::make_unique<LoggingConfigurationCborParser>();
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
}

bool LoggingConfigurationManager::Update(std::shared_ptr<LoggingConfiguration> configuration) {
    auto config = cbor_parser_->Serialize(*configuration);

    if(!cbor_configuration_service_->Save(config.get()))
        return false;

    Get(true);

    return true;
}

std::shared_ptr<LoggingConfiguration> LoggingConfigurationManager::Get(bool force_load) {
    if (configuration_ != nullptr && !force_load) {
        return configuration_;
    }

    auto config = cbor_configuration_service_->Load();
    if(!config.has_value())
        return nullptr;

    config_raw_ = std::move(config.value().config_raw);
    config_ = std::move(config.value().config);

    auto configuration = cbor_parser_->Deserialize(*config_);
    configuration_ = make_shared_ext<LoggingConfiguration>(configuration);

    return configuration_;
}

bool LoggingConfigurationManager::CreateDefaultConfiguration() {
    auto configuration = make_shared_ext<LoggingConfiguration>();

    return Update(configuration);
}

} // namespace eerie_leap::domain::logging_domain::configuration
