#include <zephyr/logging/log.h>

#include "user_com.h"

namespace eerie_leap::domain::user_com_domain {

LOG_MODULE_REGISTER(interface_logger);

UserCom::UserCom(std::shared_ptr<Modbus> modbus, std::shared_ptr<SystemConfigurationController> system_configuration_controller)
    : modbus_(modbus),
    system_configuration_controller_(system_configuration_controller) {

    k_sem_init(&processing_semaphore_, 1, 1);
    k_sem_init(&availability_semaphore_, 1, 1);

    com_users_ = std::make_shared<std::vector<ComUserConfiguration>>(
        system_configuration_controller_->Get()->com_users);
}

int UserCom::Initialize() {
    return modbus_->Initialize();
}

int UserCom::ResolveUserIds() {
    LOG_INF("Resolving server IDs...");

    uint16_t data = 0;
    Send(Modbus::SERVER_ID_ALL, ComRequestType::SET_RESOLVE_SERVER_ID, &data, sizeof(data));
    k_msleep(500);

    uint64_t server_device_id_ = 0;

    com_users_->clear();

    for(int i = 1; i < 10; i++) {
        int res = Get(i,
            ComRequestType::GET_RESOLVE_SERVER_ID_GUID,
            &server_device_id_,
            sizeof(server_device_id_));
        if(res != 0 || server_device_id_ == 0) {
            LOG_ERR("Server Device ID request failed");
            break;
        }

        LOG_INF("Resolving Server Device Id: %llu", server_device_id_);

        res = Send(i,
            ComRequestType::SET_RESOLVE_SERVER_ID_GUID,
            &server_device_id_,
            sizeof(server_device_id_));
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

bool UserCom::IsAvailable() {
    return k_sem_count_get(&availability_semaphore_) > 0 &&
        k_sem_count_get(&processing_semaphore_) > 0;
}

bool UserCom::Lock() {
    if(k_sem_count_get(&availability_semaphore_) == 0)
        return false;

    return k_sem_take(&availability_semaphore_, K_NO_WAIT) == 0;
}

void UserCom::Unlock() {
    if(k_sem_count_get(&availability_semaphore_) > 0)
        return;

    k_sem_give(&availability_semaphore_);
}

int UserCom::Get(uint8_t user_id, ComRequestType com_request_type, void* data, size_t size_bytes, k_timeout_t timeout) {
    if(k_sem_take(&processing_semaphore_, timeout) != 0)
        return -1;

    int res = modbus_->ReadHoldingRegisters(user_id, static_cast<uint16_t>(com_request_type), data, size_bytes);
    k_sem_give(&processing_semaphore_);

    return res;
}

int UserCom::Send(uint8_t user_id, ComRequestType com_request_type, void* data, size_t size_bytes, k_timeout_t timeout) {
    if(k_sem_take(&processing_semaphore_, timeout) != 0)
        return -1;

    int res = modbus_->WriteHoldingRegisters(user_id, static_cast<uint16_t>(com_request_type), data, size_bytes);
    k_sem_give(&processing_semaphore_);

    return res;
}

} // namespace eerie_leap::domain::user_com_domain
