#pragma once

#include "i_sensor_processor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorProcessor : public ISensorProcessor {
private:
    SensorReadingsFrame& sensor_readings_frame_;

public:
    explicit SensorProcessor(SensorReadingsFrame& sensor_readings_frame) :
        sensor_readings_frame_(sensor_readings_frame) {}
    void ProcessSensorReading(SensorReading& reading) override;
};

} // namespace eerie_leap::domain::sensor_domain::processors