#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "domain/user_com_domain/types/sensor_reading_dto.h"
#include "subsys/random/rng.h"

#include "com_reading_processor.h"

namespace eerie_leap::domain::user_com_domain::interface::com_reading {

using namespace eerie_leap::subsys::random;
using namespace eerie_leap::domain::user_com_domain::types;

ComReadingProcessor::ComReadingProcessor(std::shared_ptr<ComReadingInterface> com_reading_interface)
    : com_reading_interface_(std::move(com_reading_interface)) { }

void ComReadingProcessor::ProcessReading(std::shared_ptr<SensorReading> reading) {
    com_reading_interface_->SendReading(reading);
}

} // namespace eerie_leap::domain::user_com_domain::interface::com_reading
