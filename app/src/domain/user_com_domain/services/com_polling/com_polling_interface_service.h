#pragma once

#include <memory>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/user_com_domain/user_com.h"

#include "com_polling_task.hpp"

namespace eerie_leap::domain::user_com_domain::services::com_polling {

using namespace eerie_leap::domain::sensor_domain::models;

class ComPollingInterfaceService {
private:
    static constexpr int k_stack_size_ = 1024;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(6);

    k_thread_stack_t* stack_area_;
    k_work_q work_q;
    std::shared_ptr<ComPollingTask> task_;

    k_sem processing_semaphore_;
    static constexpr k_timeout_t POLLING_TIMEOUT = K_MSEC(200);

    static void PollStatusWorkTask(k_work* work);

    std::shared_ptr<UserCom> user_com_;

public:
    explicit ComPollingInterfaceService(std::shared_ptr<UserCom> user_com);
    ~ComPollingInterfaceService();

    void Initialize();
};

} // namespace eerie_leap::domain::user_com_domain::services::com_polling
