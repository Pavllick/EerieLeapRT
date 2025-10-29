#pragma once

#include <memory>

#include "subsys/gpio/i_gpio.h"
#include "domain/sensor_domain/models/sensor.h"
#include "sensor_reader_base.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::subsys::gpio;

class SensorReaderPhysicalIndicator : public SensorReaderBase {
private:
    std::shared_ptr<IGpio> gpio_;

public:
    SensorReaderPhysicalIndicator(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> readings_frame,
        std::shared_ptr<Sensor> sensor,
        std::shared_ptr<IGpio> gpio);

    ~SensorReaderPhysicalIndicator() override = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
