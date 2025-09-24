#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "com_polling_interface_service.h"

namespace eerie_leap::domain::user_com_domain::services::com_polling {

LOG_MODULE_REGISTER(com_polling_interface_logger);

ComPollingInterfaceService::ComPollingInterfaceService(std::shared_ptr<UserCom> user_com) : user_com_(std::move(user_com)) {
    k_sem_init(&processing_semaphore_, 1, 1);
    handlers_ = std::make_shared<std::unordered_map<ComUserStatus, std::shared_ptr<std::vector<ComPollingHandler>>>>();
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

    status_task_ = std::make_shared<ComStatusTask>();
    status_task_->work_q = &work_q;
    status_task_->processing_semaphore = &processing_semaphore_;
    k_work_init_delayable(&status_task_->work, ProcessStatusWorkTask);

    task_ = std::make_shared<ComPollingTask>();
    task_->work_q = &work_q;
    task_->processing_semaphore = &processing_semaphore_;
    task_->user_com = user_com_;
    task_->status_task = status_task_;
    task_->handlers = handlers_;
    task_->user_index = 0;
    task_->polling_rate_ms = K_MSEC(1000);
    k_work_init_delayable(&task_->work, PollStatusWorkTask);

    k_work_schedule_for_queue(&work_q, &task_->work, K_NO_WAIT);

    LOG_INF("Com Polling interface initialized.");
}

void ComPollingInterfaceService::PollStatusWorkTask(k_work* work) {
    ComPollingTask* task = CONTAINER_OF(work, ComPollingTask, work);

    if(task->user_com->GetUsers()->size() == 0 || task->user_index + 1 > task->user_com->GetUsers()->size()) {
        LOG_DBG("No users found.");
        return;
    }

    uint8_t server_id = task->user_com->GetUsers()->at(task->user_index).server_id;

    if(k_sem_take(task->processing_semaphore, POLLING_TIMEOUT) != 0) {
        LOG_DBG("Com Polling lock timed out for user: %d", server_id);

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
        LOG_DBG("Com Polling failed for user ID: %d", server_id);
        goto unlock;
    }

    if(task->handlers->find(user_status) != task->handlers->end()) {
        task->status_task->handlers = task->handlers->at(user_status);

        k_work_schedule_for_queue(task->status_task->work_q, &task->status_task->work, K_NO_WAIT);
    }

    if(user_status == ComUserStatus::ERROR) {
        LOG_DBG("Com Polling failed for user ID: %d", server_id);
        goto unlock;
    }

    task->user_com->Send(
        server_id,
        ComRequestType::SET_ACK,
        &user_status,
        sizeof(user_status));

    task->user_index = (task->user_index + 1) % task->user_com->GetUsers()->size();

unlock:
    task->user_com->Unlock();
    k_sem_give(task->processing_semaphore);
    k_work_reschedule_for_queue(task->work_q, &task->work, task->polling_rate_ms);
}

void ComPollingInterfaceService::ProcessStatusWorkTask(k_work* work) {
    ComStatusTask* task = CONTAINER_OF(work, ComStatusTask, work);

    for(auto handler : *task->handlers)
        handler();
}

void ComPollingInterfaceService::RegisterHandler(ComUserStatus status, ComPollingHandler handler) {
    if(handlers_->find(status) == handlers_->end())
        handlers_->insert({status, std::make_shared<std::vector<ComPollingHandler>>()});

    handlers_->at(status)->push_back(handler);
}

} // namespace eerie_leap::domain::user_com_domain::services::com_polling
