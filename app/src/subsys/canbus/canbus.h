#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <unordered_map>
#include <functional>

#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>

namespace eerie_leap::subsys::canbus {

struct CanFrame {
    uint32_t id;
    uint8_t size;
    std::array<uint8_t, CAN_MAX_DLEN> data;
};

using CanFrameHandler = std::function<void (const CanFrame&)>;

class Canbus {
private:
    const device *canbus_dev_;
    std::unordered_map<uint32_t, can_filter> can_filters_;
    std::unordered_map<uint32_t, std::vector<CanFrameHandler>> handlers_;
    bool is_initialized_ = false;

    // NOTE: Make bitrate configurable
    uint32_t bitrate_ = 500000;
    // 87.5% sampling point is recommended by CiA
    uint32_t sampling_point_percent_ = 875;

    static void CanFrameReceivedCallback(const device *dev, can_frame *frame, void *user_data);

public:
    explicit Canbus(const device *canbus_dev);
    ~Canbus();

    bool Initialize();
    bool RegisterHandler(uint32_t can_id, CanFrameHandler handler);
};

}  // namespace eerie_leap::subsys::canbus
