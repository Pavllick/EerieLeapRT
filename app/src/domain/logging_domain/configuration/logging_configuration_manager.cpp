#include "logging_configuration_manager.h"
#include "domain/logging_domain/utilities/parsers/logging_configuration_cbor_parser.h"

namespace eerie_leap::domain::logging_domain::configuration {

LOG_MODULE_REGISTER(logging_config_ctrl_logger);

LoggingConfigurationManager::LoggingConfigurationManager(ext_unique_ptr<ConfigurationService<LoggingConfig>> logging_configuration_service) :
    logging_configuration_service_(std::move(logging_configuration_service)),
    logging_config_(nullptr),
    logging_configuration_(nullptr) {

    logging_configuration_cbor_parser_ = std::make_unique<LoggingConfigurationCborParser>();
    std::shared_ptr<LoggingConfiguration> logging_configuration = nullptr;

    try {
        logging_configuration = Get(true);
    } catch(const std::exception& e) {}

    if(logging_configuration == nullptr) {
        if(!CreateDefaultConfiguration()) {
            LOG_ERR("Failed to create default Logging configuration.");
            return;
        }

        LOG_INF("Default Logging configuration loaded successfully.");
    }

    LOG_INF("Logging Configuration Manager initialized successfully.");
}

bool LoggingConfigurationManager::Update(std::shared_ptr<LoggingConfiguration> logging_configuration) {
    auto logging_config = logging_configuration_cbor_parser_->Serialize(*logging_configuration);

    if(!logging_configuration_service_->Save(logging_config.get()))
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

    auto logging_configuration = logging_configuration_cbor_parser_->Deserialize(*logging_config_);
    logging_configuration_ = make_shared_ext<LoggingConfiguration>(logging_configuration);

    return logging_configuration_;
}

bool LoggingConfigurationManager::CreateDefaultConfiguration() {
    auto logging_configuration = make_shared_ext<LoggingConfiguration>();

    return Update(logging_configuration);
}

} // namespace eerie_leap::domain::logging_domain::configuration
