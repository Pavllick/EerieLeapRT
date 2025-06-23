#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "gpio.h"
#include "gpio_emulator.h"
#include "gpio_simulator.h"

namespace eerie_leap::domain::hardware::gpio_domain {

using namespace eerie_leap::utilities::memory;

class GpioFactory {
public:
    static std::shared_ptr<IGpio> Create() {
#ifdef CONFIG_GPIO_EMUL
        return make_shared_ext<GpioEmulator>();
#elif CONFIG_GPIO
        return make_shared_ext<Gpio>();
#else
        return make_shared_ext<GpioSimulator>();
#endif
    }
};

}  // namespace eerie_leap::domain::hardware::gpio_domain
