#pragma once

#include <memory>
#include <chrono>
#include <vector>
#include <unordered_map>

#include <zephyr/kernel.h>
#include <zephyr/spinlock.h>
#include <zephyr/sys/atomic.h>

#include "subsys/canbus/canbus.h"
#include "domain/sensor_domain/sensor_readers/sensor_reader_base.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace std::chrono;
using namespace eerie_leap::subsys::canbus;

class CanbusSensorReaderRaw : public SensorReaderBase {
protected:
    std::shared_ptr<Canbus> canbus_;
    CanFrame can_frame_;
    system_clock::time_point can_frame_timestamp_;
    k_spinlock can_frame_lock_;

    std::unordered_map<uint32_t, std::vector<int>> registered_handler_ids_;

    std::shared_ptr<SensorReading> CreateRawReading();

public:
    CanbusSensorReaderRaw(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<Sensor> sensor,
        std::shared_ptr<Canbus> canbus);
    virtual ~CanbusSensorReaderRaw();

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
