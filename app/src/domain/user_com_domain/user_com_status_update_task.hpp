#pragma once

#include <vector>

#include <zephyr/kernel.h>

#include "domain/user_com_domain/user_com.h"
#include "domain/user_com_domain/types/com_user_status.h"

namespace eerie_leap::domain::user_com_domain {

using namespace eerie_leap::domain::user_com_domain::types;

struct UserComStatusUpdateTask {
    k_work work;
    k_sem* processing_semaphore;
    UserCom* user_com;
    uint8_t user_id;
    ComUserStatus user_status;
    bool success;
};

} // namespace eerie_leap::domain::user_com_domain
