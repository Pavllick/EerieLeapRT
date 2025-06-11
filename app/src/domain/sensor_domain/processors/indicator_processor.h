#pragma once

#include <memory>

#include "domain/sensor_domain/utilities/indicator_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::utilities;

class IndicatorProcessor {
private:
    std::shared_ptr<IndicatorReadingsFrame> readings_frame_;

public:
    explicit IndicatorProcessor(std::shared_ptr<IndicatorReadingsFrame> readings_frame) :
        readings_frame_(std::move(readings_frame)) {}
    void ProcessReading(std::shared_ptr<IndicatorReading> reading);
};

} // namespace eerie_leap::domain::sensor_domain::processors
