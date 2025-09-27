#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "subsys/modbus/modbus.h"
#include "domain/system_domain/system_configuration.h"
#include "domain/user_com_domain/types/com_user_status.h"
#include "domain/user_com_domain/types/com_request_type.h"
#include "controllers/system_configuration_controller.h"

namespace eerie_leap::domain::user_com_domain {

using namespace eerie_leap::subsys::modbus;
using namespace eerie_leap::controllers;

using namespace eerie_leap::domain::user_com_domain::types;
using namespace eerie_leap::domain::system_domain;

class UserCom {
private:
    static constexpr int k_stack_size_ = 1024;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(6);

    k_thread_stack_t* stack_area_;
    k_work_q work_q_;

    k_sem processing_semaphore_;
    static constexpr k_timeout_t COM_TIMEOUT = K_MSEC(200);

    static void ProcessStatusUpdateWorkTask(k_work* work);

    std::shared_ptr<Modbus> modbus_;
    std::shared_ptr<SystemConfigurationController> system_configuration_controller_;

    std::shared_ptr<std::vector<ComUserConfiguration>> com_users_;
    k_sem com_semaphore_;
    k_sem availability_semaphore_;

public:
    explicit UserCom(std::shared_ptr<Modbus> modbus, std::shared_ptr<SystemConfigurationController> system_configuration_controller);
    int Initialize();

    bool IsAvailable();
    bool Lock();
    void Unlock();

    int ResolveUserIds();
    bool IsUserAvailable(uint8_t user_id);
    int StatusUpdateNotify(ComUserStatus user_status, bool success, uint8_t user_id = Modbus::SERVER_ID_ALL);

    int Get(uint8_t user_id, ComRequestType com_request_type, void* data, size_t size_bytes, k_timeout_t timeout = K_NO_WAIT);
    int Send(uint8_t user_id, ComRequestType com_request_type, void* data, size_t size_bytes, k_timeout_t timeout = K_NO_WAIT);

    std::shared_ptr<std::vector<ComUserConfiguration>> GetUsers() { return com_users_; }
};

} // namespace eerie_leap::domain::user_com_domain
