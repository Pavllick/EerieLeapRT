#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "domain/user_com_domain/types/sensor_reading_dto.h"

#include "com_reading_interface_service.h"

namespace eerie_leap::domain::user_com_domain::services::com_reading {

using namespace eerie_leap::domain::user_com_domain::types;

LOG_MODULE_REGISTER(com_reading_interface_logger);

// #ifdef CONFIG_SHARED_MULTI_HEAP
// Z_KERNEL_STACK_DEFINE_IN(ComReadingInterfaceService::stack_area_, ComReadingInterfaceService::k_stack_size_, __attribute__((section(".ext_ram.bss"))));
// #else
// K_KERNEL_STACK_MEMBER(ComReadingInterfaceService::stack_area_, ComReadingInterfaceService::k_stack_size_);
// #endif

ComReadingInterfaceService::ComReadingInterfaceService(std::shared_ptr<UserCom> user_com) : user_com_(std::move(user_com)) {
    k_sem_init(&processing_semaphore_, 1, 1);
}

ComReadingInterfaceService::~ComReadingInterfaceService() {
    k_work_queue_stop(&work_q, K_FOREVER);
    k_thread_stack_free(stack_area_);
}

int ComReadingInterfaceService::SendReading(std::shared_ptr<SensorReading> reading, uint8_t user_id) {
    if(!user_com_->IsAvailable() || k_sem_count_get(&processing_semaphore_) == 0)
        return -1;

    task_->reading = std::move(reading);
    task_->user_id = user_id;
    task_->user_com = user_com_;

    k_work_submit_to_queue(&work_q, &task_->work);

    return 0;
}

void ComReadingInterfaceService::Initialize() {
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q.thread, "com_reading_interface");

    task_ = std::make_shared<ComReadingTask>();
    task_->processing_semaphore = &processing_semaphore_;
    k_work_init(&task_->work, SendReadingWorkTask);

    LOG_INF("Com Reading interface initialized.");
}

void ComReadingInterfaceService::SendReadingWorkTask(k_work* work) {
    ComReadingTask* task = CONTAINER_OF(work, ComReadingTask, work);

    if(k_sem_take(task->processing_semaphore, SENDING_TIMEOUT) != 0) {
        LOG_ERR("Com Send Reading lock timed out for sensor: %s", task->reading->sensor->id.c_str());

        return;
    }

    auto dto = types::SensorReadingDto::FromSensorReading(*task->reading);
    task->user_com->Send(task->user_id, ComRequestType::SET_READING, &dto, sizeof(dto));

    k_sem_give(task->processing_semaphore);
}

} // namespace eerie_leap::domain::user_com_domain::services::com_reading
