#pragma once

#include <vector>

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include "i_gpio.h"

namespace eerie_leap::subsys::gpio {

class Gpio : public IGpio {
protected:
    std::vector<gpio_dt_spec> gpio_specs_;

public:
    Gpio(std::vector<gpio_dt_spec> gpio_specs) : gpio_specs_(gpio_specs) {}

    int Initialize() override;
    bool ReadChannel(int channel) override;
    int GetChannelCount() override;
};

}  // namespace eerie_leap::subsys::gpio
