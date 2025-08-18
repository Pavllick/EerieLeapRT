#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "utilities/guid/guid_generator.h"
#include "subsys/modbus/modbus.h"

#include "types/request_type.h"

namespace eerie_leap::domain::interface_domain {

using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::subsys::modbus;
using namespace eerie_leap::domain::interface_domain::types;

class Interface {
private:
    std::shared_ptr<Modbus> modbus_;
    std::shared_ptr<GuidGenerator> guid_generator_;

    std::shared_ptr<std::vector<uint8_t>> server_ids_;

    int Get(uint8_t server_id, RequestType request_type, void* data, size_t size_bytes);
    int Set(uint8_t server_id, RequestType request_type, void* data, size_t size_bytes);

public:
    explicit Interface(std::shared_ptr<Modbus> modbus);
    int Initialize();

    int ServerIdResolver();

    std::shared_ptr<std::vector<uint8_t>> GetServerIds() { return server_ids_; }
};

} // namespace eerie_leap::domain::interface_domain
