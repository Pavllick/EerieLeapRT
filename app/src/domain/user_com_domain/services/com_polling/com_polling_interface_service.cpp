#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "domain/user_com_domain/types/com_user_status.h"
#include "com_polling_interface_service.h"

namespace eerie_leap::domain::user_com_domain::services::com_polling {

using namespace eerie_leap::domain::user_com_domain::types;

LOG_MODULE_REGISTER(com_polling_interface_logger);

ComPollingInterfaceService::ComPollingInterfaceService(std::shared_ptr<UserCom> user_com) : user_com_(std::move(user_com)) {
    k_sem_init(&processing_semaphore_, 1, 1);
}

ComPollingInterfaceService::~ComPollingInterfaceService() {
    k_work_queue_stop(&work_q, K_FOREVER);
    k_thread_stack_free(stack_area_);
}

void ComPollingInterfaceService::Initialize() {
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q.thread, "com_polling_interface");

    task_ = std::make_shared<ComPollingTask>();
    task_->work_q = &work_q;
    task_->processing_semaphore = &processing_semaphore_;
    task_->user_com = user_com_;
    task_->user_index = 0;
    task_->polling_rate_ms = K_MSEC(1000);

    k_work_init_delayable(&task_->work, PollStatusWorkTask);

    k_work_schedule_for_queue(&work_q, &task_->work, K_NO_WAIT);

    LOG_INF("Com Polling interface initialized.");
}

void ComPollingInterfaceService::PollStatusWorkTask(k_work* work) {
    ComPollingTask* task = CONTAINER_OF(work, ComPollingTask, work);
    uint8_t server_id = task->user_com->GetUsers()->at(task->user_index).server_id;

    if(k_sem_take(task->processing_semaphore, POLLING_TIMEOUT) != 0) {
        LOG_ERR("Com Polling lock timed out for user: %d", server_id);

        return;
    }

    task->user_com->Lock();

    auto user_status = ComUserStatus::ERROR;
    int res = task->user_com->Get(
        server_id,
        ComRequestType::GET_STATUS,
        &user_status,
        sizeof(user_status),
        K_MSEC(100));

    if(res != 0) {
        LOG_ERR("Com Polling failed for user: %d", server_id);
        goto unlock;
    }

    LOG_INF("User %d status: %d", server_id, user_status);

    task->user_index = (task->user_index + 1) % task->user_com->GetUsers()->size();

    if(user_status == ComUserStatus::ERROR)
        LOG_ERR("User %d status: %d", server_id, user_status);

    if(user_status != ComUserStatus::ERROR)
        task->user_com->Send(
            server_id,
            ComRequestType::SET_ACK,
            &user_status,
            sizeof(user_status));

unlock:
    task->user_com->Unlock();
    k_sem_give(task->processing_semaphore);
    k_work_reschedule_for_queue(task->work_q, &task->work, task->polling_rate_ms);
}

} // namespace eerie_leap::domain::user_com_domain::services::com_polling
