#include <zephyr/canbus/isotp.h>
#include <zephyr/logging/log.h>

#include "canbus.h"

LOG_MODULE_REGISTER(canbus_logger);

namespace eerie_leap::subsys::canbus {

Canbus::Canbus(const device *canbus_dev) : canbus_dev_(canbus_dev) {}

int Canbus::Initialize() {
    if (!device_is_ready(canbus_dev_)) {
		LOG_ERR("CANBus device driver not ready.");
		return -1;
	}

    int ret = can_set_mode(canbus_dev_, CAN_MODE_NORMAL);
	if (ret != 0) {
		LOG_ERR("CANBus failed to set mode [%d]", ret);
		return ret;
	}

	ret = can_start(canbus_dev_);
	if (ret != 0) {
		LOG_ERR("CANBus failed to start device [%d]", ret);
		return ret;
	}

    return 0;
}

}  // namespace eerie_leap::subsys::canbus
