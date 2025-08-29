#include "com_reading_processor.h"

namespace eerie_leap::domain::user_com_domain::interfaces::com_reading {

ComReadingProcessor::ComReadingProcessor(std::shared_ptr<ComReadingInterface> com_reading_interface)
    : com_reading_interface_(std::move(com_reading_interface)) { }

void ComReadingProcessor::ProcessReading(std::shared_ptr<SensorReading> reading) {
    com_reading_interface_->SendReading(reading);
}

} // namespace eerie_leap::domain::user_com_domain::interfaces::com_reading
