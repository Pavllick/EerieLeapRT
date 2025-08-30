#pragma once

#include <memory>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/processors/i_reading_processor.h"
#include "domain/user_com_domain/services/com_reading/com_reading_interface_service.h"

namespace eerie_leap::domain::user_com_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::user_com_domain::services::com_reading;

class ComReadingProcessor : public IReadingProcessor {
private:
    std::shared_ptr<ComReadingInterfaceService> com_reading_interface_service_;

public:
    explicit ComReadingProcessor(std::shared_ptr<ComReadingInterfaceService> com_reading_interface_service);

    void ProcessReading(std::shared_ptr<SensorReading> reading) override;
};

} // namespace eerie_leap::domain::user_com_domain::processors
