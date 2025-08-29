#include "log_reading_processor.h"

namespace eerie_leap::domain::logging_domain::processors {

using namespace eerie_leap::domain::logging_domain::services;

LogReadingProcessor::LogReadingProcessor(std::shared_ptr<LogWriterService> log_writer_service)
    : log_writer_service_(std::move(log_writer_service)) { }

void LogReadingProcessor::ProcessReading(std::shared_ptr<SensorReading> reading) {
    log_writer_service_->LogReading(reading);
}

} // namespace eerie_leap::domain::logging_domain::processors
