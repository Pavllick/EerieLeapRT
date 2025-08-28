#include <zephyr/kernel.h>

#include "domain/user_com_domain/types/sensor_reading_dto.h"
#include "subsys/random/rng.h"

#include "com_reading_interface.h"

namespace eerie_leap::domain::user_com_domain::interface {

using namespace eerie_leap::subsys::random;
using namespace eerie_leap::domain::user_com_domain::types;

ComReadingInterface::ComReadingInterface(std::shared_ptr<UserCom> user_com) : user_com_(std::move(user_com)) {}

int ComReadingInterface::SendReading(const SensorReading& reading, uint8_t user_id) {
    auto dto = types::SensorReadingDto::FromSensorReading(reading);

    return user_com_->Send(user_id, RequestType::SET_READING, &dto, sizeof(dto));
}

} // namespace eerie_leap::domain::user_com_domain::interface
