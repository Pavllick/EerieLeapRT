#pragma once

#include <optional>
#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "i_adc.h"
#include "adc_configuration.h"

namespace eerie_leap::domain::adc_domain::hardware {

class AdcSimulator : public IAdc {
private:
    std::optional<AdcConfiguration> adc_config_;

public:
    AdcSimulator() = default;
    ~AdcSimulator() = default;

    int Initialize() override;
    void UpdateConfiguration(AdcConfiguration config) override;
    double ReadChannel(int channel) override;
};

}  // namespace eerie_leap::domain::adc_domain::hardware
