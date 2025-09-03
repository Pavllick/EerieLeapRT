#pragma once

#include <memory>

#include <zephyr/kernel.h>

#include "domain/user_com_domain/user_com.h"

namespace eerie_leap::domain::user_com_domain::services::com_polling {

using namespace eerie_leap::domain::user_com_domain;

struct ComPollingTask {
    k_work_q* work_q;
    k_work_delayable work;
    k_sem* processing_semaphore;
    std::shared_ptr<UserCom> user_com;
    int user_index;
    k_timeout_t polling_rate_ms;
};

} // namespace eerie_leap::domain::user_com_domain::services::com_polling
