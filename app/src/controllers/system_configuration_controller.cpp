#include "system_configuration_controller.h"

namespace eerie_leap::controllers {

bool SystemConfigurationController::Update(std::shared_ptr<SystemConfiguration> system_configuration) {
    SystemConfig system_config {
        .hw_version = system_configuration->hw_version,
        .sw_version = system_configuration->sw_version
    };

    if(!system_configuration_service_->Save(system_config))
        return false;

    system_config_ = std::make_shared<SystemConfig>(system_config);
    system_configuration_ = system_configuration;

    return true;
}

std::shared_ptr<SystemConfiguration> SystemConfigurationController::Get() {
    if (system_configuration_ != nullptr) {
        return system_configuration_;
    }

    auto system_config = system_configuration_service_->Load();
    if(!system_config.has_value())
        return nullptr;

    SystemConfiguration system_configuration {
        .hw_version = system_config->hw_version,
        .sw_version = system_config->sw_version
    };
    system_configuration_ = std::make_shared<SystemConfiguration>(system_configuration);

    system_config_ = std::make_shared<SystemConfig>(system_config.value());

    return system_configuration_;
}

} // namespace eerie_leap::controllers
