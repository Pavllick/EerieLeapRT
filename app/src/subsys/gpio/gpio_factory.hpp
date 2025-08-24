#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "domain/device_tree/dt_gpio.h"
#include "gpio.h"
#include "gpio_emulator.h"
#include "gpio_simulator.h"

namespace eerie_leap::subsys::gpio {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::device_tree;

class GpioFactory {
public:
    static std::shared_ptr<IGpio> Create() {
#ifdef CONFIG_GPIO_EMUL
        if(DtGpio::Get().has_value())
            return make_shared_ext<GpioEmulator>(DtGpio::Get().value());
#endif

#ifdef CONFIG_GPIO
        if(DtGpio::Get().has_value())
            return make_shared_ext<Gpio>(DtGpio::Get().value());
#endif

        return make_shared_ext<GpioSimulator>();
    }
};

}  // namespace eerie_leap::subsys::gpio
