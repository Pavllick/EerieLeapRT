#pragma once

#include <zephyr/random/random.h>

#include "i_gpio.h"

namespace eerie_leap::subsys::gpio {

class GpioSimulator : public IGpio {
public:
    GpioSimulator() = default;
    ~GpioSimulator() = default;

    int Initialize() override;
    bool ReadChannel(int channel) override;
    int GetChannelCount() override;
};

}  // namespace eerie_leap::subsys::gpio
