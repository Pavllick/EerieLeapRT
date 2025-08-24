#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "domain/device_tree/dt_adc.h"

#include "i_adc_manager.h"
#include "adc_emulator.h"
#include "adc_simulator.h"
#include "adc_manager.h"

namespace eerie_leap::subsys::adc {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::device_tree;

class AdcFactory {
public:
    static std::shared_ptr<IAdcManager> Create() {
#ifdef CONFIG_ADC_EMUL
        if(DtAdc::Get().has_value())
            return make_shared_ext<AdcEmulatorManager>(DtAdc::Get().value());
#endif

#ifdef CONFIG_ADC
        if(DtAdc::Get().has_value())
            return make_shared_ext<AdcManager>(DtAdc::Get().value());
#endif

        return make_shared_ext<AdcSimulatorManager>();
    }
};

}  // namespace eerie_leap::subsys::adc
