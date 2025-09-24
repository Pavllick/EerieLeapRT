#pragma once

#include <memory>

#include <zephyr/kernel.h>

#include "domain/user_com_domain/services/com_polling/com_polling_handler.h"

namespace eerie_leap::domain::user_com_domain::services::com_polling {

struct ComStatusTask {
    k_work_q* work_q;
    k_work_delayable work;
    k_sem* processing_semaphore;
    std::shared_ptr<std::vector<ComPollingHandler>> handlers;
};

} // namespace eerie_leap::domain::user_com_domain::services::com_polling
