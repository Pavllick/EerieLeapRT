#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "domain/device_tree/device_tree_setup.h"

#include "i_adc_manager.h"
#include "adc_emulator.h"
#include "adc_simulator.h"
#include "adc_manager.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::device_tree;

class AdcFactory {
public:
    static std::shared_ptr<IAdcManager> Create() {
#ifdef CONFIG_ADC_EMUL
        if(DeviceTreeSetup::Get().GetAdcInfos().has_value())
            return make_shared_ext<AdcEmulatorManager>(DeviceTreeSetup::Get().GetAdcInfos().value());
#endif

#ifdef CONFIG_ADC
        if(DeviceTreeSetup::Get().GetAdcInfos().has_value())
            return make_shared_ext<AdcManager>(DeviceTreeSetup::Get().GetAdcInfos().value());
#endif

        return make_shared_ext<AdcSimulatorManager>();
    }
};

}  // namespace eerie_leap::domain::hardware::adc_domain
