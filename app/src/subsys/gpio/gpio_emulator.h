#pragma once

#include <vector>

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include "gpio.h"

namespace eerie_leap::subsys::gpio {

#define GPIOC0_NODE DT_ALIAS(gpioc)

class GpioEmulator : public Gpio {
private:
    std::vector<gpio_dt_spec> gpio_specs_;

public:
    GpioEmulator(std::vector<gpio_dt_spec> gpio_specs) : Gpio(gpio_specs), gpio_specs_(gpio_specs) {}
    virtual ~GpioEmulator() = default;

    int Initialize() override;
    bool ReadChannel(int channel) override;
    int GetChannelCount() override;
};

}  // namespace eerie_leap::subsys::gpio
