#pragma once

#include <memory>

#include "domain/sensor_domain/models/sensor.h"
#include "sensor_reader_base.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

class SensorReaderVirtualIndicator : public SensorReaderBase {
public:
    SensorReaderVirtualIndicator(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> readings_frame,
        std::shared_ptr<Sensor> sensor);

    ~SensorReaderVirtualIndicator() override = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
