#pragma once

#include "subsys/adc/models/adc_configuration.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

using namespace eerie_leap::subsys::adc::models;

class AdcConfigurationValidator {
private:
    static void ValidateSamples(const AdcConfiguration& configuration);
    static void ValidateChannels(const AdcConfiguration& configuration);

public:
    static void Validate(const AdcConfiguration& configuration);
};

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
