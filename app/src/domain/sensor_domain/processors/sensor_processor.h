#pragma once

#include <memory>

#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorProcessor {
private:
    std::shared_ptr<SensorReadingsFrame> readings_frame_;

public:
    explicit SensorProcessor(std::shared_ptr<SensorReadingsFrame> readings_frame);

    void ProcessReading(std::shared_ptr<SensorReading> reading);
};

} // namespace eerie_leap::domain::sensor_domain::processors
