#include <zephyr/logging/log.h>

#include "dt_logger.h"

#include "dt_canbus.h"

namespace eerie_leap::subsys::device_tree {

LOG_MODULE_DECLARE(dt_logger);

std::unordered_map<uint8_t, const device*> DtCanbus::canbus_devs_;

void DtCanbus::Initialize() {
#if defined(CONFIG_CAN) && DT_HAS_CHOSEN(zephyr_canbus)
    auto canbus_dev = DEVICE_DT_GET(CANBUS_NODE);
    canbus_devs_.insert({0, canbus_dev});
    LOG_INF("Canbus initialized.");
#endif
}

const device* DtCanbus::Get(uint8_t bus_channel) {
    if(!canbus_devs_.contains(bus_channel))
        return nullptr;

    return canbus_devs_.at(bus_channel);
}

} // namespace eerie_leap::subsys::device_tree
