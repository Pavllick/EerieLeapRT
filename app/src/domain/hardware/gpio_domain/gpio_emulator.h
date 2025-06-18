#pragma once

#include <vector>

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include "gpio.h"

namespace eerie_leap::domain::hardware::gpio_domain {

#define GPIOC0_NODE DT_ALIAS(gpioc)

class GpioEmulator : public Gpio {
private:
    std::vector<gpio_dt_spec> gpio_specs_;

public:
    GpioEmulator() {
    #if DT_HAS_ALIAS(gpioc)
        gpio_specs_ = { DT_FOREACH_CHILD_SEP(GPIOC0_NODE, GPIO_SPEC, (,)) };
    #endif
    }
    virtual ~GpioEmulator() = default;

    int Initialize() override;
    bool ReadChannel(int channel) override;
    int GetChannelCount() override;
};

}  // namespace eerie_leap::domain::hardware::gpio_domain
