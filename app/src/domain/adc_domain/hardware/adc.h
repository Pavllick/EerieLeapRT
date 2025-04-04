#pragma once

#include <optional>
#include <stdio.h>
#include "adc_config.h"

namespace eerie_leap::domain::adc_domain::hardware {

class Adc {
private:
    std::optional<AdcConfig> adc_config_;

public:
    Adc() = default;
    ~Adc() = default;
    
    int Initialize();
    void UpdateConfiguration(AdcConfig config);
    int ReadChannel(int channel);
};

}  // namespace eerie_leap::domain::adc_domain::hardware