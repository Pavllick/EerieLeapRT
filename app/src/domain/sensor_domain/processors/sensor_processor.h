#pragma once

#include <memory>

#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/i_reading_processor.h"
#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorProcessor : public IReadingProcessor {
private:
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;

public:
    explicit SensorProcessor(std::shared_ptr<SensorReadingsFrame> sensor_readings_frame);

    void ProcessReading(std::shared_ptr<SensorReading> reading) override;
};

} // namespace eerie_leap::domain::sensor_domain::processors
