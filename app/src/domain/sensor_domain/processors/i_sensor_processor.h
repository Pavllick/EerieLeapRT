#pragma once

#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;

class ISensorProcessor {
public:
    virtual void ProcessSensorReading(SensorReading& reading) = 0;
    virtual ~ISensorProcessor() = default;
};

} // namespace eerie_leap::domain::sensor_domain::processors