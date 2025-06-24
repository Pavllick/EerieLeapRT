#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "i_adc_manager.h"
#include "adc_emulator.h"
#include "adc_simulator.h"
#include "adc_manager.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::utilities::memory;

class AdcFactory {
public:
    static std::shared_ptr<IAdcManager> Create() {
#ifdef CONFIG_ADC_EMUL
        return make_shared_ext<AdcEmulatorManager>();
#elif CONFIG_ADC
        return make_shared_ext<AdcManager>();
#else
        return make_shared_ext<AdcSimulatorManager>();
#endif
    }
};

}  // namespace eerie_leap::domain::hardware::adc_domain
