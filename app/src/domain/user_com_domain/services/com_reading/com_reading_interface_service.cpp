#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "domain/user_com_domain/types/sensor_reading_dto.h"

#include "com_reading_interface_service.h"

namespace eerie_leap::domain::user_com_domain::services::com_reading {

using namespace eerie_leap::domain::user_com_domain::types;

LOG_MODULE_REGISTER(com_reading_interface_logger);

ComReadingInterfaceService::ComReadingInterfaceService(std::shared_ptr<UserCom> user_com, std::shared_ptr<SensorReadingsFrame> sensor_readings_frame)
    : user_com_(std::move(user_com)), sensor_readings_frame_(std::move(sensor_readings_frame)) {

    k_sem_init(&processing_semaphore_, 1, 1);
}

ComReadingInterfaceService::~ComReadingInterfaceService() {
    k_work_sync sync;
    k_work_cancel_delayable_sync(&task_->work, &sync);

    k_work_queue_stop(&work_q, K_FOREVER);
    k_thread_stack_free(stack_area_);
}

void ComReadingInterfaceService::Initialize() {
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q.thread, "com_reading_interface");

    task_ = std::make_shared<ComReadingTask>();
    task_->work_q = &work_q;
    task_->processing_semaphore = &processing_semaphore_;
    task_->sending_interval_ms = SENDING_INTERVAL_MS;
    task_->user_id = Modbus::SERVER_ID_ALL;
    task_->user_com = user_com_;
    task_->sensor_readings_frame = sensor_readings_frame_;
    k_work_init_delayable(&task_->work, SendReadingWorkTask);
    k_work_schedule_for_queue(&work_q, &task_->work, K_NO_WAIT);

    LOG_INF("Com Reading interface initialized.");
}

void ComReadingInterfaceService::SendReadingWorkTask(k_work* work) {
    ComReadingTask* task = CONTAINER_OF(work, ComReadingTask, work);

    if(k_sem_take(task->processing_semaphore, SENDING_TIMEOUT) != 0) {
        LOG_ERR("Com Send Reading lock timed out.");
        return;
    }

    int retry_count = 0;
    int retry_interval_ms = 2;
    const int max_retries = task->sending_interval_ms / retry_interval_ms;
    while(!task->user_com->IsAvailable() && retry_count < max_retries) {
        k_msleep(retry_interval_ms);
        retry_count++;
    }

    if(task->user_com->IsAvailable()) {
        for(const auto& [sensor_id, reading] : task->sensor_readings_frame->GetReadings()) {
            if(reading->status != ReadingStatus::PROCESSED)
                continue;

            auto dto = types::SensorReadingDto::FromSensorReading(*reading);
            task->user_com->Send(task->user_id, ComRequestType::SET_READING, &dto, sizeof(dto));
        }
    }

    k_sem_give(task->processing_semaphore);
    k_work_reschedule_for_queue(task->work_q, &task->work, K_MSEC(task->sending_interval_ms));
}

} // namespace eerie_leap::domain::user_com_domain::services::com_reading
