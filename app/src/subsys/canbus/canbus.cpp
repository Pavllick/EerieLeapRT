#include <zephyr/logging/log.h>

#include "canbus.h"

LOG_MODULE_REGISTER(canbus_logger);

namespace eerie_leap::subsys::canbus {

Canbus::Canbus(const device *canbus_dev) : canbus_dev_(canbus_dev) {}

Canbus::~Canbus() {
    can_stop(canbus_dev_);
}

bool Canbus::Initialize() {
    if(!device_is_ready(canbus_dev_)) {
		LOG_ERR("CANBus device driver not ready.");
		return false;
	}

    can_mode_t capabilities;
    int ret = can_get_capabilities(canbus_dev_, &capabilities);
    if(ret != 0) {
        LOG_ERR("Failed to get capabilities.");
        return false;
    }

    can_mode_t can_mode = CAN_MODE_NORMAL;
    if(capabilities & CAN_MODE_FD)
        can_mode = CAN_MODE_FD;

    ret = can_set_mode(canbus_dev_, can_mode);
	if(ret != 0) {
		LOG_ERR("CANBus failed to set mode [%d].", ret);
		return false;
	}

    can_timing timing;
    ret = can_calc_timing(canbus_dev_, &timing, bitrate_, sampling_point_percent_);
    if(ret > 0) {
        LOG_INF("Sample-Point error: %d.", ret);
    }

    if(ret < 0) {
        LOG_ERR("Failed to calc a valid timing.");
        return false;
    }

    ret = can_set_timing(canbus_dev_, &timing);
    if(ret != 0) {
        LOG_ERR("Failed to set timing.");
        return false;
    }

	ret = can_start(canbus_dev_);
	if(ret != 0) {
		LOG_ERR("CANBus failed to start device [%d].", ret);
		return false;
	}

    LOG_INF("CANBus initialized successfully.");

    is_initialized_ = true;

    return true;
}

void Canbus::CanFrameReceivedCallback(const device *dev, can_frame *frame, void *user_data) {
    Canbus *canbus = static_cast<Canbus*>(user_data);

    CanFrame can_frame = {
        .id = frame->id,
        .size = frame->dlc
    };

    std::copy(frame->data, frame->data + frame->dlc, can_frame.data.begin());

    for(const auto& handler : canbus->handlers_.at(frame->id))
        handler(can_frame);
}

bool Canbus::RegisterHandler(uint32_t can_id, CanFrameHandler handler) {
    if(!is_initialized_) {
        LOG_ERR("CANBus is not initialized.");
        return false;
    }

    if(!handlers_.contains(can_id))
        handlers_.insert({can_id, {}});
    handlers_.at(can_id).push_back(std::move(handler));

    if(!can_filters_.contains(can_id)) {
        can_filter filter = {
            .id = can_id,
            .mask = CAN_STD_ID_MASK,
            .flags = 0
        };

        int filter_id = can_add_rx_filter(canbus_dev_, CanFrameReceivedCallback, this, &filter);
        if(filter_id < 0) {
            LOG_ERR("Unable to add rx filter [%d].", filter_id);
            return false;
        }

        can_filters_.insert({can_id, filter});
    }

    return true;
}

}  // namespace eerie_leap::subsys::canbus
