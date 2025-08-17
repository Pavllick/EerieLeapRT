#include <zephyr/logging/log.h>

#include "gpio.h"

LOG_MODULE_REGISTER(gpio_logger);

namespace eerie_leap::subsys::gpio {

int Gpio::Initialize() {
    LOG_INF("Gpio initialization started.");

    for(auto& gpio_spec : gpio_specs_) {
        if (!gpio_is_ready_dt(&gpio_spec)) {
            LOG_ERR("Gpio device %s is not ready", gpio_spec.port->name);

            return -1;
        }

        gpio_pin_configure(gpio_spec.port, gpio_spec.pin, GPIO_INPUT | GPIO_PULL_UP | GPIO_ACTIVE_LOW);

        LOG_INF("Gpio channel %d configured", gpio_spec.pin);
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
