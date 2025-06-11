#pragma once

#include <memory>

#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/utilities/indicator_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorProcessor {
private:
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<IndicatorReadingsFrame> indicator_readings_frame_;

public:
    SensorProcessor(std::shared_ptr<SensorReadingsFrame> sensor_readings_frame, std::shared_ptr<IndicatorReadingsFrame> indicator_readings_frame) :
        sensor_readings_frame_(std::move(sensor_readings_frame)), indicator_readings_frame_(std::move(indicator_readings_frame)) {}
    void ProcessReading(std::shared_ptr<SensorReading> reading);
};

} // namespace eerie_leap::domain::sensor_domain::processors
