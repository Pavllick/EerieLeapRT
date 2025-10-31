#pragma once

#include <memory>

#include <zephyr/kernel.h>
#include <zephyr/spinlock.h>
#include <zephyr/sys/atomic.h>

#include "subsys/canbus/canbus.h"
#include "subsys/dbc/dbc.h"
#include "domain/sensor_domain/sensor_readers/sensor_reader_base.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::subsys::dbc;

class CanbusSensorReader : public SensorReaderBase {
private:
    std::shared_ptr<Canbus> canbus_;
    std::shared_ptr<Dbc> dbc_;
    CanFrame can_frame_;
    k_spinlock can_frame_lock_;

public:
    CanbusSensorReader(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<Sensor> sensor,
        std::shared_ptr<Canbus> canbus,
        std::shared_ptr<Dbc> dbc);
    virtual ~CanbusSensorReader() = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
