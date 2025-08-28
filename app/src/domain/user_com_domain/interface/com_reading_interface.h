#pragma once

#include <memory>

#include "domain/sensor_domain/models/sensor_reading.h"

#include "domain/user_com_domain/user_com.h"

namespace eerie_leap::domain::user_com_domain::interface {

using namespace eerie_leap::domain::sensor_domain::models;

class ComReadingInterface {
private:
    std::shared_ptr<UserCom> user_com_;

public:
    explicit ComReadingInterface(std::shared_ptr<UserCom> user_com);

    int SendReading(const SensorReading& reading, uint8_t user_id = Modbus::SERVER_ID_ALL);
};

} // namespace eerie_leap::domain::user_com_domain::interface
