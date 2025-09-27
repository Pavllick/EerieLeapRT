#include <zephyr/logging/log.h>

#include "user_com.h"
#include "user_com_status_update_task.hpp"

namespace eerie_leap::domain::user_com_domain {

LOG_MODULE_REGISTER(interface_logger);

UserCom::UserCom(std::shared_ptr<Modbus> modbus, std::shared_ptr<SystemConfigurationController> system_configuration_controller)
    : modbus_(modbus),
    system_configuration_controller_(system_configuration_controller) {

    k_sem_init(&processing_semaphore_, 1, 1);
    k_sem_init(&com_semaphore_, 1, 1);
    k_sem_init(&availability_semaphore_, 1, 1);

    com_users_ = std::make_shared<std::vector<ComUserConfiguration>>(
        system_configuration_controller_->Get()->com_users);
}

int UserCom::Initialize() {
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    k_work_queue_init(&work_q_);
    k_work_queue_start(&work_q_, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q_.thread, "user_com_interface");

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

bool UserCom::IsUserAvailable(uint8_t user_id) {
    for(auto user : *com_users_) {
        if(user.server_id == user_id)
            return true;
    }

    return false;
}

int UserCom::StatusUpdateNotify(ComUserStatus user_status, bool success, uint8_t user_id) {
    UserComStatusUpdateTask* task = new UserComStatusUpdateTask();
    k_work_init(&task->work, ProcessStatusUpdateWorkTask);

    task->processing_semaphore = &processing_semaphore_;
    task->user_com = this;
    task->user_id = user_id;
    task->user_status = user_status;
    task->success = success;

    k_work_submit_to_queue(&work_q_, &task->work);

    return 0;
}

void UserCom::ProcessStatusUpdateWorkTask(k_work* work) {
    UserComStatusUpdateTask* task = CONTAINER_OF(work, UserComStatusUpdateTask, work);
    int res = 0;

    if(k_sem_take(task->processing_semaphore, COM_TIMEOUT) != 0) {
        LOG_DBG("Com Status Update lock timed out for user: %d", task->user_id);
        goto unlock;
    }

    if(task->user_id != Modbus::SERVER_ID_ALL && !task->user_com->IsUserAvailable(task->user_id)) {
        LOG_DBG("User ID: %d not found.", task->user_id);
        goto unlock;
    }

    task->user_com->Lock();

    res = task->user_com->Send(
        task->user_id,
        task->success ? ComRequestType::SET_STATUS_UPDATE_OK : ComRequestType::SET_STATUS_UPDATE_FAIL,
        &task->user_status,
        sizeof(task->user_status),
        K_MSEC(100));

    if(res != 0) {
        LOG_DBG("Com Status Update failed for user ID: %d", task->user_id);
        goto unlock;
    }

unlock:
    task->user_com->Unlock();
    k_sem_give(task->processing_semaphore);
    delete task;
}

bool UserCom::IsAvailable() {
    return k_sem_count_get(&availability_semaphore_) > 0 &&
        k_sem_count_get(&com_semaphore_) > 0;
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
    if(k_sem_take(&com_semaphore_, timeout) != 0)
        return -1;

    int res = modbus_->ReadHoldingRegisters(user_id, static_cast<uint16_t>(com_request_type), data, size_bytes);
    k_sem_give(&com_semaphore_);

    return res;
}

int UserCom::Send(uint8_t user_id, ComRequestType com_request_type, void* data, size_t size_bytes, k_timeout_t timeout) {
    if(k_sem_take(&com_semaphore_, timeout) != 0)
        return -1;

    int res = modbus_->WriteHoldingRegisters(user_id, static_cast<uint16_t>(com_request_type), data, size_bytes);
    k_sem_give(&com_semaphore_);

    return res;
}

} // namespace eerie_leap::domain::user_com_domain
