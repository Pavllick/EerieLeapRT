#include <zephyr/logging/log.h>

#include "user_com.h"

namespace eerie_leap::domain::user_com_domain {

LOG_MODULE_REGISTER(interface_logger);

UserCom::UserCom(std::shared_ptr<Modbus> modbus, std::shared_ptr<SystemConfigurationController> system_configuration_controller)
    : modbus_(modbus),
    system_configuration_controller_(system_configuration_controller) {

        com_users_ = std::make_shared<std::vector<ComUserConfiguration>>(
            system_configuration_controller_->Get()->com_users
        );
    }

int UserCom::Initialize() {
    return modbus_->Initialize();
}

int UserCom::ResolveUserIds() {
    LOG_INF("Resolving server IDs...");

    uint16_t data = 0;
    Send(Modbus::SERVER_ID_ALL, RequestType::SET_RESOLVE_SERVER_ID, &data, sizeof(data));
    k_msleep(500);

    uint64_t server_device_id_ = 0;

    for(int i = 1; i < 10; i++) {
        int res = Get(i, RequestType::GET_RESOLVE_SERVER_ID_GUID, &server_device_id_, sizeof(server_device_id_));
        if(res != 0 || server_device_id_ == 0) {
            LOG_ERR("Server Device ID request failed");
            break;
        }

        LOG_INF("Resolving Server Device Id: %llu", server_device_id_);

        res = Send(i, RequestType::SET_RESOLVE_SERVER_ID_GUID, &server_device_id_, sizeof(server_device_id_));
        if(res != 0) {
            LOG_ERR("Server Device ID confirmation failed");
            return res;
        }

        com_users_->push_back({
            .device_id = server_device_id_,
            .server_id = static_cast<uint16_t>(i)
        });
    }

    system_configuration_controller_->UpdateComUsers(*com_users_);

    return 0;
}

int UserCom::Get(uint8_t user_id, RequestType request_type, void* data, size_t size_bytes) {
    return modbus_->ReadHoldingRegisters(user_id, static_cast<uint16_t>(request_type), data, size_bytes);
}

int UserCom::Send(uint8_t user_id, RequestType request_type, void* data, size_t size_bytes) {
    return modbus_->WriteHoldingRegisters(user_id, static_cast<uint16_t>(request_type), data, size_bytes);
}

} // namespace eerie_leap::domain::user_com_domain
