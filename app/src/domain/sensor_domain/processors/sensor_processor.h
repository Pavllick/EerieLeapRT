#pragma once

#include <memory>

#include "i_sensor_processor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorProcessor : public ISensorProcessor {
private:
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;

public:
    explicit SensorProcessor(std::shared_ptr<SensorReadingsFrame> sensor_readings_frame) :
        sensor_readings_frame_(std::move(sensor_readings_frame)) {}
    void ProcessSensorReading(std::shared_ptr<SensorReading> reading) override;
};

} // namespace eerie_leap::domain::sensor_domain::processors