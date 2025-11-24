#pragma once

#include "utilities/memory/heap_allocator.h"
#include "configuration/json/configs/json_adc_config.h"
#include "subsys/adc/models/adc_configuration.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::subsys::adc::models;

class AdcConfigurationJsonParser {
public:
    AdcConfigurationJsonParser() = default;

    ext_unique_ptr<JsonAdcConfig> Serialize(const AdcConfiguration& adc_configuration);
    AdcConfiguration Deserialize(const JsonAdcConfig& json);
};

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
