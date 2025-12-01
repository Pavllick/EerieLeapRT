#pragma once

#include <memory>

#include "subsys/device_tree/dt_gpio.h"
#include "gpio.h"
#include "gpio_emulator.h"
#include "gpio_simulator.h"

namespace eerie_leap::subsys::gpio {

using namespace eerie_leap::subsys::device_tree;

class GpioFactory {
public:
    static std::shared_ptr<IGpio> Create() {
#ifdef CONFIG_GPIO_EMUL
        if(DtGpio::Get().has_value())
            return std::make_shared<GpioEmulator>(DtGpio::Get().value());
#endif

#ifdef CONFIG_GPIO
        if(DtGpio::Get().has_value())
            return std::make_shared<Gpio>(DtGpio::Get().value());
#endif

        return std::make_shared<GpioSimulator>();
    }
};

}  // namespace eerie_leap::subsys::gpio
