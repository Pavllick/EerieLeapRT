#include <zephyr/logging/log.h>

#include "gpio.h"

LOG_MODULE_REGISTER(gpio_logger);

namespace eerie_leap::subsys::gpio {

int Gpio::Initialize() {
    LOG_INF("Gpio initialization started.");

    for(int i = 0; i < gpio_specs_.size(); ++i) {
        if (!gpio_is_ready_dt(&gpio_specs_[i])) {
            LOG_ERR("Gpio device %s is not ready", gpio_specs_[i].port->name);

            return -1;
        }

        gpio_pin_configure(gpio_specs_[i].port, gpio_specs_[i].pin, GPIO_INPUT | GPIO_PULL_UP | GPIO_ACTIVE_HIGH);

        LOG_INF("Gpio channel %d configured", i);
    }

    LOG_INF("Gpio initialized successfully.");

    return 0;
}

bool Gpio::ReadChannel(int channel) {
    return gpio_pin_get_dt(&gpio_specs_[channel]);
}

int Gpio::GetChannelCount() {
    return gpio_specs_.size();
}

}  // namespace eerie_leap::subsys::gpio
