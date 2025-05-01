#pragma once

#include <optional>
#include <stdio.h>
#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "adc.h"
#include "adc_config.h"

namespace eerie_leap::domain::adc_domain::hardware {

class AdcSimulator : public IAdc {
private:
    std::optional<AdcConfig> adc_config_;

public:
    AdcSimulator() = default;
    ~AdcSimulator() = default;
    
    int Initialize() override;
    void UpdateConfiguration(AdcConfig config) override;
    double ReadChannel(int channel) override;
};

}  // namespace eerie_leap::domain::adc_domain::hardware