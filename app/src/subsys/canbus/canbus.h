#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <unordered_map>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/sys/atomic.h>

#include "canbus_type.h"
#include "can_frame.h"

namespace eerie_leap::subsys::canbus {

using CanFrameHandler = std::function<void (const CanFrame&)>;

class Canbus {
private:
    const device *canbus_dev_;
    std::unordered_map<uint32_t, can_filter> can_filters_;
    std::unordered_map<uint32_t, std::vector<CanFrameHandler>> handlers_;

    bool is_initialized_ = false;
    CanbusType type_;
    bool is_extended_id_ = false;
    uint32_t bitrate_;
    uint32_t data_bitrate_;
    bool bitrate_detected_ = false;
    atomic_t auto_detect_running_;
    std::function<void (uint32_t bitrate)> bitrate_detected_fn_;

    static constexpr k_timeout_t FRAME_SEND_TIMEOUT_MS = K_MSEC(2);
    static constexpr uint32_t AUTO_DETECT_TIMEOUT_MS = 500;
    static constexpr uint32_t MIN_FRAMES_FOR_DETECTION = 3;

    static constexpr int k_stack_size_ = 2048;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(6);

    k_thread_stack_t* stack_area_ = nullptr;
    k_thread thread_data_;

    // Ordered by most common first
    static constexpr std::array<uint32_t, 9> classical_can_supported_bitrates_ = {
        500000, 1000000, 250000, 125000, 100000, 83333, 50000, 20000, 10000,
    };

    // Ordered by most common first
    static constexpr std::array<uint32_t, 13> canfd_supported_bitrates_ = {
        500000, 1000000, 250000, 125000, 100000, 83333, 50000, 20000, 10000,
        2000000, 4000000, 5000000, 8000000
    };

    void ActivityMonitorThreadEntry();

    bool StartActivityMonitoring();
    void StopActivityMonitoring();
    bool AutoDetectBitrate();
    bool TestBitrate(uint32_t bitrate, uint32_t &frame_count);

    static void SendFrameCallback(const device* dev, int error, void* user_data);
    bool SetTiming(uint32_t bitrate);
    bool SetDataTiming(uint32_t bitrate);
    bool RegisterFilter(uint32_t can_id);
    static void CanFrameReceivedCallback(const device *dev, can_frame *frame, void *user_data);
    void PrintCanLimits();
    void PrintCanFdLimits();

public:
    using BitrateDetectedCallback = std::function<void (uint32_t bitrate)>;

    Canbus(
        const device *canbus_dev,
        CanbusType type,
        uint32_t bitrate,
        uint32_t data_bitrate = 0,
        bool is_extended_id = false);
    ~Canbus();

    bool Initialize();
    bool RegisterFrameReceivedHandler(uint32_t can_id, CanFrameHandler handler);

    CanbusType GetType() const { return type_; }
    void SendFrame(const CanFrame& frame);
    uint32_t GetDetectedBitrate() const { return bitrate_; }
    bool IsBitrateDetected() const { return bitrate_detected_; }
    void RegisterBitrateDetectedCallback(const BitrateDetectedCallback& callback);

    static bool IsBitrateSupported(CanbusType type, uint32_t bitrate);
};

}  // namespace eerie_leap::subsys::canbus
