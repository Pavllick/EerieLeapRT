#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_adc_config/cbor_adc_config.h"
#include "subsys/adc/models/adc_configuration.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::adc::models;

class AdcConfigurationCborParser {
public:
    AdcConfigurationCborParser() = default;

    ext_unique_ptr<CborAdcConfig> Serialize(const AdcConfiguration& adc_configuration);
    AdcConfiguration Deserialize(const CborAdcConfig& adc_config);
};

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
