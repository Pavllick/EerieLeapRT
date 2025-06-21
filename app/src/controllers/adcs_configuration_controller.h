#pragma once

#include <memory>

#include "configuration/adc_config/adc_config.h"
#include "configuration/services/configuration_service.h"
#include "domain/hardware/adc_domain/models/adc_configuration.h"
#include "domain/hardware/adc_domain/adc_manager.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::hardware::adc_domain;
using namespace eerie_leap::domain::hardware::adc_domain::models;

class AdcsConfigurationController {
private:
    std::shared_ptr<ConfigurationService<AdcsConfig>> adcs_configuration_service_;
    std::shared_ptr<IAdcManager> adc_manager_;
    std::shared_ptr<ExtVector> adcs_config_raw_;
    std::shared_ptr<AdcsConfig> adcs_config_;
    std::shared_ptr<std::vector<std::shared_ptr<AdcConfiguration>>> adcs_configuration_;

public:
    AdcsConfigurationController(std::shared_ptr<ConfigurationService<AdcsConfig>> adcs_configuration_service, std::shared_ptr<IAdcManager> adc_manager);

    bool Update(const std::shared_ptr<std::vector<std::shared_ptr<AdcConfiguration>>>& adcs_configuration);
    std::shared_ptr<IAdcManager> Get(bool force_load = false);
};

} // namespace eerie_leap::controllers
