#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/adc_config/adc_config.h"
#include "configuration/services/configuration_service.h"
#include "subsys/adc/models/adc_configuration.h"
#include "subsys/adc/i_adc_manager.h"
#include "domain/sensor_domain/utilities/parsers/adc_configuration_cbor_parser.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::subsys::adc::models;
using namespace eerie_leap::domain::sensor_domain::utilities::parsers;

class AdcConfigurationManager {
private:
    ext_unique_ptr<ConfigurationService<AdcConfig>> adc_configuration_service_;
    std::shared_ptr<IAdcManager> adc_manager_;
    ext_unique_ptr<ExtVector> adc_config_raw_;
    ext_unique_ptr<AdcConfig> adc_config_;
    std::shared_ptr<AdcConfiguration> adc_configuration_;
    std::unique_ptr<AdcConfigurationCborParser> adc_configuration_cbor_parser_;

    bool CreateDefaultConfiguration();

public:
    AdcConfigurationManager(ext_unique_ptr<ConfigurationService<AdcConfig>> adc_configuration_service);

    bool Update(const AdcConfiguration& adc_configuration);
    std::shared_ptr<IAdcManager> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::sensor_domain::configuration
