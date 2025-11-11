#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/adc_config/adc_config.h"
#include "subsys/adc/models/adc_configuration.h"

namespace eerie_leap::domain::sensor_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::adc::models;

class AdcCborParser {
public:
    AdcCborParser() = default;

    ext_unique_ptr<AdcConfig> Serialize(const AdcConfiguration& adc_configuration);
    const AdcConfiguration Deserialize(const AdcConfig& adc_config);
};

} // namespace eerie_leap::domain::sensor_domain::utilities::parsers
