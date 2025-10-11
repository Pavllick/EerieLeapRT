#pragma once

#include <memory>

#include "configuration/adc_config/adc_config.h"
#include "configuration/services/configuration_service.h"
#include "subsys/adc/models/adc_configuration.h"
#include "subsys/adc/i_adc_manager.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::configuration::services;
using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::subsys::adc::models;

class AdcConfigurationManager {
private:
    std::shared_ptr<ConfigurationService<AdcConfig>> adc_configuration_service_;
    std::shared_ptr<IAdcManager> adc_manager_;
    std::shared_ptr<ExtVector> adc_config_raw_;
    std::shared_ptr<AdcConfig> adc_config_;
    std::shared_ptr<AdcConfiguration> adc_configuration_;

    void SetDefaultConfiguration();

public:
    AdcConfigurationManager(std::shared_ptr<ConfigurationService<AdcConfig>> adc_configuration_service);

    bool Update(const AdcConfiguration& adc_configuration);
    std::shared_ptr<IAdcManager> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::sensor_domain::configuration
