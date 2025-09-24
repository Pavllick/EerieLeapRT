#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <zephyr/kernel.h>

#include "domain/user_com_domain/user_com.h"
#include "domain/user_com_domain/services/com_polling/com_polling_handler.h"
#include "domain/user_com_domain/types/com_user_status.h"
#include "com_status_task.hpp"

namespace eerie_leap::domain::user_com_domain::services::com_polling {

using namespace eerie_leap::domain::user_com_domain;

struct ComPollingTask {
    k_work_q* work_q;
    k_work_delayable work;
    k_sem* processing_semaphore;
    std::shared_ptr<UserCom> user_com;
    std::shared_ptr<ComStatusTask> status_task;
    std::shared_ptr<std::unordered_map<ComUserStatus, std::shared_ptr<std::vector<ComPollingHandler>>>> handlers;
    int user_index;
    k_timeout_t polling_rate_ms;
};

} // namespace eerie_leap::domain::user_com_domain::services::com_polling
