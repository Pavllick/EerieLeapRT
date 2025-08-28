#pragma once

#include <memory>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/processors/i_reading_processor.h"
#include "com_reading_interface.h"

namespace eerie_leap::domain::user_com_domain::interface::com_reading {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::processors;

class ComReadingProcessor : public IReadingProcessor {
private:
    std::shared_ptr<ComReadingInterface> com_reading_interface_;

public:
    explicit ComReadingProcessor(std::shared_ptr<ComReadingInterface> com_reading_interface);

    void ProcessReading(std::shared_ptr<SensorReading> reading) override;
};

} // namespace eerie_leap::domain::user_com_domain::interface::com_reading
