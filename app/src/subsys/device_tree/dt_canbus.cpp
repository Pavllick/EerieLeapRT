#include <zephyr/logging/log.h>

#include "dt_logger.h"

#include "dt_canbus.h"

namespace eerie_leap::subsys::device_tree {

LOG_MODULE_DECLARE(dt_logger);

const device* DtCanbus::canbus_dev_ = nullptr;

void DtCanbus::Initialize() {
#if defined(CONFIG_CAN) && DT_HAS_CHOSEN(zephyr_canbus)
    canbus_dev_ = DEVICE_DT_GET(CANBUS_NODE);
    LOG_INF("Canbus initialized.");
#endif
}

} // namespace eerie_leap::subsys::device_tree
