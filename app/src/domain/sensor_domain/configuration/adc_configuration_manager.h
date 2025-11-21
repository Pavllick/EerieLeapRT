#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_adc_config/cbor_adc_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "configuration/json/configs/json_adc_config.h"
#include "configuration/services/json_configuration_service.h"
#include "subsys/adc/models/adc_configuration.h"
#include "subsys/adc/i_adc_manager.h"
#include "domain/sensor_domain/configuration/utilities/parsers/adc_configuration_cbor_parser.h"
#include "domain/sensor_domain/configuration/utilities/parsers/adc_configuration_json_parser.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::subsys::adc::models;
using namespace eerie_leap::domain::sensor_domain::configuration::utilities::parsers;

class AdcConfigurationManager {
private:
    ext_unique_ptr<CborConfigurationService<CborAdcConfig>> cbor_configuration_service_;
    ext_unique_ptr<JsonConfigurationService<JsonAdcConfig>> json_configuration_service_;

    std::unique_ptr<AdcConfigurationCborParser> cbor_parser_;
    std::unique_ptr<AdcConfigurationJsonParser> json_parser_;

    std::shared_ptr<IAdcManager> adc_manager_;
    std::shared_ptr<AdcConfiguration> configuration_;

    uint32_t json_config_checksum_;

    bool ApplyJsonConfiguration();
    bool CreateDefaultConfiguration();

public:
    AdcConfigurationManager(
        ext_unique_ptr<CborConfigurationService<CborAdcConfig>> cbor_configuration_service,
        ext_unique_ptr<JsonConfigurationService<JsonAdcConfig>> json_configuration_service);

    bool Update(const AdcConfiguration& configuration);
    std::shared_ptr<IAdcManager> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::sensor_domain::configuration
