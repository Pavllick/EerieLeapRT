#pragma once

#include <memory>

#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;

class IReadingProcessor {
public:
    virtual ~IReadingProcessor() = default;

    virtual void ProcessReading(std::shared_ptr<SensorReading> reading) = 0;
};

} // namespace eerie_leap::domain::sensor_domain::processors
