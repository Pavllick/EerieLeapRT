#pragma once

#include <memory>

#include "gpio.h"
#include "gpio_emulator.h"
#include "gpio_simulator.h"

namespace eerie_leap::domain::hardware::gpio_domain {

class GpioFactory {
public:
    static std::shared_ptr<IGpio> Create() {
#ifdef CONFIG_GPIO_EMUL
        return std::make_shared<GpioEmulator>();
#elif CONFIG_GPIO
        return std::make_shared<Gpio>();
#else
        return std::make_shared<GpioSimulator>();
#endif
    }
};

}  // namespace eerie_leap::domain::hardware::gpio_domain
