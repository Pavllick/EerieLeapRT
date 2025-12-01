#pragma once

#include <memory>

#include "subsys/device_tree/dt_adc.h"

#include "i_adc_manager.h"
#include "adc_emulator.h"
#include "adc_simulator.h"
#include "adc_manager.h"

namespace eerie_leap::subsys::adc {

using namespace eerie_leap::subsys::device_tree;

class AdcFactory {
public:
    static std::shared_ptr<IAdcManager> Create() {
#ifdef CONFIG_ADC_EMUL
        if(DtAdc::Get().has_value())
            return std::make_shared<AdcEmulatorManager>(DtAdc::Get().value());
#endif

#ifdef CONFIG_ADC
        if(DtAdc::Get().has_value())
            return std::make_shared<AdcManager>(DtAdc::Get().value());
#endif

        return std::make_shared<AdcSimulatorManager>();
    }
};

}  // namespace eerie_leap::subsys::adc
