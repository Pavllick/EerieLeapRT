#include "com_reading_processor.h"

namespace eerie_leap::domain::user_com_domain::processors {

ComReadingProcessor::ComReadingProcessor(std::shared_ptr<ComReadingInterfaceService> com_reading_interface_service)
    : com_reading_interface_service_(std::move(com_reading_interface_service)) { }

void ComReadingProcessor::ProcessReading(std::shared_ptr<SensorReading> reading) {
    com_reading_interface_service_->SendReading(reading);
}

} // namespace eerie_leap::domain::user_com_domain::processors
