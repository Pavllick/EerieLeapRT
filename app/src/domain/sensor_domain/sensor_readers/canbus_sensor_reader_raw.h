#pragma once

#include <memory>

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include "subsys/canbus/canbus.h"
#include "domain/sensor_domain/sensor_readers/sensor_reader_base.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::subsys::canbus;

class CanbusSensorReaderRaw : public SensorReaderBase {
private:
    std::shared_ptr<Canbus> canbus_;
    CanFrame canbus_frame_;

public:
    CanbusSensorReaderRaw(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<Sensor> sensor,
        std::shared_ptr<Canbus> canbus);
    virtual ~CanbusSensorReaderRaw() = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
