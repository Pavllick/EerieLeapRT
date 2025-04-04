#include <zephyr/logging/log.h>

#include "adc.h"

namespace eerie_leap::domain::adc_domain {

LOG_MODULE_REGISTER(adc_logger);

int Adc::Initialize() { 
    LOG_INF("Adc initialization started");

    return 0;
}

int Adc::ReadChannel(int channel) {
    if (channel < 0 || channel > 7) {
        return -1;
    }

    return channel * 10;
}

}  // namespace eerie_leap::domain::adc_domain