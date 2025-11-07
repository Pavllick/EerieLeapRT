#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <functional>

#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>

#include "can_frame.h"

namespace eerie_leap::subsys::canbus {

using CanFrameHandler = std::function<void (const CanFrame&)>;

class Canbus {
private:
    const device *canbus_dev_;
    std::unordered_map<uint32_t, can_filter> can_filters_;
    std::unordered_map<uint32_t, std::vector<CanFrameHandler>> handlers_;
    bool is_initialized_ = false;

    uint32_t bitrate_;
    uint32_t sampling_point_percent_;

    static void CanFrameReceivedCallback(const device *dev, can_frame *frame, void *user_data);

public:
    explicit Canbus(const device *canbus_dev, uint32_t bitrate, uint32_t sampling_point_percent);
    ~Canbus();

    bool Initialize();
    bool RegisterHandler(uint32_t can_id, CanFrameHandler handler);
};

}  // namespace eerie_leap::subsys::canbus
