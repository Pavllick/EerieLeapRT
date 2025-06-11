#pragma once

#include <memory>

#include "adc.h"
#include "adc_emulator.h"
#include "adc_simulator.h"

namespace eerie_leap::domain::hardware::adc_domain {

class AdcFactory {
public:
    static std::shared_ptr<IAdc> Create() {
#ifdef CONFIG_ADC_EMUL
        return std::make_shared<AdcEmulator>();
#elif CONFIG_ADC
        return std::make_shared<Adc>();
#else
        return std::make_shared<AdcSimulator>();
#endif
    }
};

}  // namespace eerie_leap::domain::hardware::adc_domain
