#include <zephyr/logging/log.h>

#include "interface.h"

namespace eerie_leap::domain::interface_domain {

LOG_MODULE_REGISTER(interface_logger);

Interface::Interface(std::shared_ptr<Modbus> modbus) : modbus_(modbus), server_ids_(std::make_shared<std::vector<uint8_t>>()) { }

int Interface::Initialize() {
    return modbus_->Initialize();
}

int Interface::ServerIdResolver() {
    uint16_t data = 0;
    LOG_INF("Server ID Resole set");
    Set(Modbus::SERVER_ID_ALL, RequestType::GET_RESOLVE_SERVER_ID, &data, sizeof(data));

    k_msleep(500);

    Guid guid = {
        .device_hash = 0,
        .counter = 0,
        .timestamp = 0,
    };

    for(int i = 1; i < 10; i++) {
        int res = Get(i, RequestType::GET_RESOLVE_SERVER_ID_GUID, &guid, sizeof(guid));
        if(res != 0) {
            LOG_ERR("Server ID Guid request failed");
            return res;
        }

        LOG_INF("Server GUID: %llu", guid.AsUint64());

        res = Set(i, RequestType::SET_RESOLVE_SERVER_ID_GUID, &guid, sizeof(guid));
        if(res != 0) {
            LOG_ERR("Server ID Guid set failed");
            return res;
        }

        server_ids_->push_back(i);
    }

    return 0;
}

int Interface::Get(uint8_t server_id, RequestType request_type, void* data, size_t size_bytes) {
    return modbus_->ReadHoldingRegisters(server_id, static_cast<uint16_t>(request_type), data, size_bytes);
}

int Interface::Set(uint8_t server_id, RequestType request_type, void* data, size_t size_bytes) {
    return modbus_->WriteHoldingRegisters(server_id, static_cast<uint16_t>(request_type), data, size_bytes);
}

} // namespace eerie_leap::domain::interface_domain
