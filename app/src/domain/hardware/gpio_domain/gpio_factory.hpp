#pragma once

#include <memory>

#include "gpio_simulator.h"

namespace eerie_leap::domain::hardware::gpio_domain {

class GpioFactory {
public:
    static std::shared_ptr<IGpio> Create() {
        return std::make_shared<GpioSimulator>();
    }
};

}  // namespace eerie_leap::domain::hardware::gpio_domain
