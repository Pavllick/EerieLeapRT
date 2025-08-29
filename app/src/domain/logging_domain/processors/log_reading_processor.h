#pragma once

#include <memory>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/processors/i_reading_processor.h"
#include "domain/logging_domain/services/log_writer_service.h"

namespace eerie_leap::domain::logging_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::logging_domain::services;

class LogReadingProcessor : public IReadingProcessor {
private:
    std::shared_ptr<LogWriterService> log_writer_service_;

public:
    explicit LogReadingProcessor(std::shared_ptr<LogWriterService> log_writer_service);

    void ProcessReading(std::shared_ptr<SensorReading> reading) override;
};

} // namespace eerie_leap::domain::logging_domain::processors
