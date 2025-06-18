#pragma once

#include <memory>

#include "configuration/adc_config/adc_config.h"
#include "configuration/services/configuration_service.hpp"
#include "domain/hardware/adc_domain/adc_configuration.h"
#include "utilities/logging/logger.hpp"

namespace eerie_leap::controllers {

using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::hardware::adc_domain;
using eerie_leap::utilities::logging::Logger;

class AdcConfigurationController {
private:
    std::shared_ptr<ConfigurationService<AdcConfig>> adc_configuration_service_;
    std::shared_ptr<ExtVector> adc_config_raw_;
    std::shared_ptr<AdcConfig> adc_config_;
    std::shared_ptr<AdcConfiguration> adc_configuration_;
    Logger logger_;

public:
    explicit AdcConfigurationController(std::shared_ptr<ConfigurationService<AdcConfig>> adc_configuration_service);

    bool Update(std::shared_ptr<AdcConfiguration> adc_configuration);
    std::shared_ptr<AdcConfiguration> Get(bool force_load = false);
};

} // namespace eerie_leap::controllers
