#include <stdexcept>
#include <span>
#include <algorithm>
#include <memory>

#include <zephyr/logging/log.h>

#include "canbus.h"

LOG_MODULE_REGISTER(canbus_logger);

namespace eerie_leap::subsys::canbus {

Canbus::Canbus(const device *canbus_dev, CanbusType type, uint32_t bitrate)
    : canbus_dev_(canbus_dev), bitrate_(bitrate), auto_detect_running_(ATOMIC_INIT(0)), type_(type) {}

Canbus::~Canbus() {
    StopActivityMonitoring();
    can_stop(canbus_dev_);

    if(stack_area_)
        k_thread_stack_free(stack_area_);
}

bool Canbus::Initialize() {
    if(!device_is_ready(canbus_dev_)) {
		LOG_ERR("Device driver not ready.");
		return false;
	}

    can_mode_t capabilities;
    int ret = can_get_capabilities(canbus_dev_, &capabilities);
    if(ret != 0) {
        LOG_ERR("Failed to get capabilities.");
        return false;
    }

    can_mode_t can_mode = CAN_MODE_NORMAL;
    if(type_ == CanbusType::CANFD && (capabilities & CAN_MODE_FD))
        can_mode = CAN_MODE_FD;
    else
        type_ = CanbusType::CLASSICAL_CAN;

    ret = can_set_mode(canbus_dev_, can_mode);
	if(ret != 0) {
		LOG_ERR("Failed to set mode [%d].", ret);
		return false;
	}

    if(bitrate_ == 0) {
        LOG_INF("Auto-bitrate mode enabled - will detect on bus activity");

        stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
        if(!StartActivityMonitoring()) {
            LOG_ERR("Failed to start activity monitoring.");
            return false;
        }
    } else {
        if(!SetTiming(bitrate_)) {
            LOG_ERR("Failed to set timing.");
            return false;
        }

        ret = can_start(canbus_dev_);
        if(ret != 0) {
            LOG_ERR("Failed to start device [%d].", ret);
            return false;
        }

        bitrate_detected_ = true;
    }

    LOG_INF("CANBus initialized successfully.");
    is_initialized_ = true;

    return true;
}

bool Canbus::SetTiming(uint32_t bitrate) {
    if(bitrate == 0)
        return false;

    int ret = can_set_bitrate(canbus_dev_, bitrate);
    if(ret != 0)
        return false;

    return true;
}

void Canbus::SendFrame(const CanFrame& frame) {
    if(!is_initialized_ || !bitrate_detected_)
        return;

    struct can_frame can_frame = {
        .id = frame.id,
        .dlc = static_cast<uint8_t>(frame.data.size()),
    };
    memcpy(can_frame.data, frame.data.data(), frame.data.size());

    int res = can_send(
        canbus_dev_,
        &can_frame,
        FRAME_SEND_TIMEOUT_MS,
        SendFrameCallback,
        nullptr);

    if(res != 0) {
        // LOG_ERR("Failed to send frame [%d].", res);
        return;
    }
}

void Canbus::SendFrameCallback(const device* dev, int error, void* user_data) {
    if(error != 0)
        LOG_INF("SendFrameCallback error: %d", error);
}

void Canbus::CanFrameReceivedCallback(const device *dev, can_frame *frame, void *user_data) {
    auto* canbus = static_cast<Canbus*>(user_data);

    CanFrame can_frame = {
        .id = frame->id
    };

    can_frame.data.resize(frame->dlc);
    std::copy(frame->data, frame->data + frame->dlc, can_frame.data.begin());

    if(canbus->handlers_.contains(frame->id)) {
        for(const auto& handler : canbus->handlers_.at(frame->id))
            handler(can_frame);
    }
}

bool Canbus::RegisterFrameReceivedHandler(uint32_t can_id, CanFrameHandler handler) {
    if(!is_initialized_) {
        LOG_ERR("CANBus is not initialized.");
        return false;
    }

    if(!handlers_.contains(can_id))
        handlers_.insert({can_id, {}});
    handlers_.at(can_id).push_back(std::move(handler));

    if(atomic_get(&auto_detect_running_) && !bitrate_detected_)
        return true;

    return RegisterFilter(can_id);
}

bool Canbus::RegisterFilter(uint32_t can_id) {
    if(!is_initialized_) {
        LOG_ERR("CANBus is not initialized.");
        return false;
    }

    if(!handlers_.contains(can_id))
        throw std::runtime_error("Handler not found for ID " + std::to_string(can_id));

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

bool Canbus::StartActivityMonitoring() {
    if(!stack_area_) {
        LOG_ERR("Activity monitor stack not allocated.");
        return false;
    }

    atomic_set(&auto_detect_running_, 1);

    k_thread_create(
        &thread_data_,
        stack_area_,
        k_stack_size_,
        [](void* instance, void* p2, void* p3) {
            static_cast<Canbus*>(instance)->ActivityMonitorThreadEntry(); },
        this, nullptr, nullptr,
        k_priority_, 0, K_NO_WAIT);

    k_thread_name_set(&thread_data_, "can_activity_monitor");

    return true;
}

void Canbus::StopActivityMonitoring() {
    if(atomic_get(&auto_detect_running_)) {
        atomic_set(&auto_detect_running_, 0);
        k_thread_join(&thread_data_, K_FOREVER);
    }
}

void Canbus::ActivityMonitorThreadEntry() {
    LOG_INF("CANBus auto-detection started.");

    while(atomic_get(&auto_detect_running_) && !bitrate_detected_) {
        if(AutoDetectBitrate()) {
            LOG_INF("Bitrate successfully detected: %u bps", bitrate_);

            if(bitrate_detected_fn_)
                bitrate_detected_fn_(bitrate_);

            for(const auto& [can_id, _] : handlers_)
                RegisterFilter(can_id);

            break;
        }

        k_sleep(K_MSEC(CONFIG_EERIE_LEAP_CANBUS_AUTO_DETECT_INTERVAL_MS));
    }

    LOG_INF("CANBus auto-detection stopped.");
}

bool Canbus::AutoDetectBitrate() {
    std::span<const uint32_t> supported_bitrates;
    if(type_ == CanbusType::CANFD)
        supported_bitrates = canfd_supported_bitrates_;
    else
        supported_bitrates = classical_can_supported_bitrates_;

    for(int i = 0; i < supported_bitrates.size(); i++) {
        if(!atomic_get(&auto_detect_running_)) {
            LOG_WRN("Bitrate detection stopped by user");
            return false;
        }

        uint32_t frame_count = 0;
        if(TestBitrate(supported_bitrates[i], frame_count)) {
            bitrate_detected_ = true;
            bitrate_ = supported_bitrates[i];

            return true;
        }

        can_stop(canbus_dev_);
        k_sleep(K_MSEC(50));
    }

    return false;
}

bool Canbus::IsBitrateSupported(CanbusType type, uint32_t bitrate) {
    if(bitrate == 0)
        return true;

    if(type == CanbusType::CANFD)
        return std::ranges::find(canfd_supported_bitrates_, bitrate) != canfd_supported_bitrates_.end();
    else
        return std::ranges::find(classical_can_supported_bitrates_, bitrate) != classical_can_supported_bitrates_.end();
}

bool Canbus::TestBitrate(uint32_t bitrate, uint32_t &frame_count) {
    if (!SetTiming(bitrate))
        return false;

    int ret = can_start(canbus_dev_);
    if (ret != 0) {
        LOG_WRN("Failed to start CAN for bitrate %u [%d]", bitrate, ret);
        return false;
    }

    can_filter filter = {
        .id = 0,
        .mask = 0,  // Accept all IDs
        .flags = 0
    };

    volatile uint32_t received_frames = 0;

    int filter_id = can_add_rx_filter(canbus_dev_,
        [](const device *dev, can_frame *frame, void *user_data) {
            volatile uint32_t *counter = static_cast<volatile uint32_t*>(user_data);
            (*counter)++;
        },
        (void*)&received_frames,
        &filter);

    if(filter_id < 0) {
        LOG_WRN("Failed to add test filter [%d]", filter_id);
        can_stop(canbus_dev_);

        return false;
    }

    k_sleep(K_MSEC(AUTO_DETECT_TIMEOUT_MS));
    can_remove_rx_filter(canbus_dev_, filter_id);
    frame_count = received_frames;

    if(received_frames >= MIN_FRAMES_FOR_DETECTION) {
        enum can_state state;
        struct can_bus_err_cnt err_cnt;

        int ret = can_get_state(canbus_dev_, &state, &err_cnt);
        if(ret != 0)
            return false;

        // Valid activity means error-active state with reasonable error counts
        return (state == CAN_STATE_ERROR_ACTIVE &&
                err_cnt.tx_err_cnt < 128 &&
                err_cnt.rx_err_cnt < 128);
    }

    return false;
}

void Canbus::RegisterBitrateDetectedCallback(const BitrateDetectedCallback& callback) {
    bitrate_detected_fn_ = callback;
}

}  // namespace eerie_leap::subsys::canbus
