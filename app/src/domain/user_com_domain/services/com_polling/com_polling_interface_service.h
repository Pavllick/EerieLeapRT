#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "domain/user_com_domain/user_com.h"
#include "domain/user_com_domain/types/com_user_status.h"

#include "com_polling_task.hpp"
#include "com_status_task.hpp"
#include "com_polling_handler.h"

namespace eerie_leap::domain::user_com_domain::services::com_polling {

using namespace eerie_leap::domain::user_com_domain::types;

class ComPollingInterfaceService {
private:
    static constexpr int k_stack_size_ = 1024 * 3;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(6);

    k_thread_stack_t* stack_area_;
    k_work_q work_q_;
    std::shared_ptr<ComPollingTask> task_;
    std::shared_ptr<ComStatusTask> status_task_;

    k_sem processing_semaphore_;
    static constexpr k_timeout_t POLLING_TIMEOUT = K_MSEC(200);

    std::shared_ptr<std::unordered_map<ComUserStatus, std::shared_ptr<std::vector<ComPollingHandler>>>> handlers_;

    static void PollStatusWorkTask(k_work* work);
    static void ProcessStatusWorkTask(k_work* work);

    std::shared_ptr<UserCom> user_com_;

public:
    explicit ComPollingInterfaceService(std::shared_ptr<UserCom> user_com);
    ~ComPollingInterfaceService();

    void Initialize();
    void RegisterHandler(ComUserStatus status, ComPollingHandler handler);
};

} // namespace eerie_leap::domain::user_com_domain::services::com_polling
