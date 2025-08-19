#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "utilities/guid/guid_generator.h"
#include "subsys/modbus/modbus.h"
#include "domain/sensor_domain/models/sensor_reading.h"

#include "types/request_type.h"

namespace eerie_leap::domain::user_com_domain {

using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::subsys::modbus;
using namespace eerie_leap::domain::sensor_domain::models;

using namespace eerie_leap::domain::user_com_domain::types;

class UserCom {
private:
    std::shared_ptr<Modbus> modbus_;
    std::shared_ptr<GuidGenerator> guid_generator_;

    std::shared_ptr<std::vector<uint8_t>> user_ids_;

    int Get(uint8_t user_id, RequestType request_type, void* data, size_t size_bytes);
    int Set(uint8_t user_id, RequestType request_type, void* data, size_t size_bytes);

public:
    explicit UserCom(std::shared_ptr<Modbus> modbus);
    int Initialize();

    int SendReading(const SensorReading& reading, uint8_t user_id = Modbus::SERVER_ID_ALL);
    int ServerIdResolver();

    std::shared_ptr<std::vector<uint8_t>> GetUserIds() { return user_ids_; }
};

} // namespace eerie_leap::domain::user_com_domain
