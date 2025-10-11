#include <zephyr/logging/log.h>

#include "dt_logger.h"

#include "dt_gpio.h"

namespace eerie_leap::subsys::device_tree {

LOG_MODULE_DECLARE(dt_logger);

std::optional<std::vector<gpio_dt_spec>> DtGpio::gpio_specs_ = std::nullopt;

void DtGpio::Initialize() {
#if DT_HAS_ALIAS(gpioc)
    gpio_specs_ = { DT_FOREACH_CHILD_SEP(GPIOC0_NODE, GPIO_SPEC, (,)) };
    LOG_INF("GPIO initialized.");
#endif
}

} // namespace eerie_leap::subsys::device_tree
