#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "subsys/modbus/modbus.h"
#include "domain/system_domain/system_configuration.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "controllers/system_configuration_controller.h"

#include "types/request_type.h"

namespace eerie_leap::domain::user_com_domain {

using namespace eerie_leap::subsys::modbus;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::controllers;

using namespace eerie_leap::domain::user_com_domain::types;
using namespace eerie_leap::domain::system_domain;

class UserCom {
private:
    std::shared_ptr<Modbus> modbus_;
    std::shared_ptr<SystemConfigurationController> system_configuration_controller_;

    std::shared_ptr<std::vector<ComUserConfiguration>> com_users_;

    int Get(uint8_t user_id, RequestType request_type, void* data, size_t size_bytes);
    int Set(uint8_t user_id, RequestType request_type, void* data, size_t size_bytes);

public:
    explicit UserCom(std::shared_ptr<Modbus> modbus, std::shared_ptr<SystemConfigurationController> system_configuration_controller);
    int Initialize();

    int SendReading(const SensorReading& reading, uint8_t user_id = Modbus::SERVER_ID_ALL);
    int ResolveUserIds();

    std::shared_ptr<std::vector<ComUserConfiguration>> GetUsers() { return com_users_; }
};

} // namespace eerie_leap::domain::user_com_domain
