#include <string>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "subsys/random/rng.h"

#include "domain/logging_domain/models/log_metadata.h"
#include "domain/logging_domain/models/log_record.h"

#include "log_writer_service.h"

namespace eerie_leap::domain::logging_domain::services {

using namespace eerie_leap::subsys::random;
using namespace eerie_leap::domain::logging_domain::models;

LOG_MODULE_REGISTER(log_writer_service_logger);

#ifdef CONFIG_SHARED_MULTI_HEAP
Z_KERNEL_STACK_DEFINE_IN(LogWriterService::stack_area_, LogWriterService::k_stack_size_, __attribute__((section(".ext_ram.bss"))));
#else
K_KERNEL_STACK_MEMBER(LogWriterService::stack_area_, LogWriterService::k_stack_size_);
#endif

LogWriterService::LogWriterService(std::shared_ptr<IFsService> fs_service, std::shared_ptr<ITimeService> time_service)
    : fs_service_(std::move(fs_service)), time_service_(std::move(time_service)), logger_running_(ATOMIC_INIT(0)) {
    k_sem_init(&processing_semaphore_, 1, 1);
}

int LogWriterService::LogReading(std::shared_ptr<SensorReading> reading) {
    if(!atomic_get(&logger_running_))
        return -1;

    if(k_sem_count_get(&processing_semaphore_) == 0)
        return -1;

    task_->reading = std::move(reading);

    k_work_submit_to_queue(&work_q, &task_->work);

    return 0;
}

void LogWriterService::Initialize() {
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, stack_area_,
        K_THREAD_STACK_SIZEOF(stack_area_),
        k_priority_, nullptr);

    k_thread_name_set(&work_q.thread, "log_writer_service");

    task_ = std::make_shared<LogWriterTask>();
    task_->processing_semaphore = &processing_semaphore_;
    task_->fs_service = fs_service_;
    task_->time_service = time_service_;
    k_work_init(&task_->work, LogReadingWorkTask);

    LOG_INF("Log writer service initialized.");
}

std::string LogWriterService::GetNewFileName(const system_clock::time_point& tp) {
    return "data_logs/" +
        std::to_string(TimeHelpers::ToUint32(tp)) +
        "_" +
        std::to_string(Rng::Get16()) +
        ".ell";
}

int LogWriterService::LogWriterStart() {
    auto current_time = time_service_->GetCurrentTime();
    auto log_metadata = LogMetadata::Create(current_time);

    int res = 0;

    if(!fs_service_->Exists("data_logs"))
        if(!fs_service_->CreateDirectory("data_logs")) {
            LOG_ERR("Failed to create data_logs directory");
            return -1;
        }

    std::string file_name;
    for(int i = 0; i < 10; i++) {
        auto new_file_name = GetNewFileName(current_time);
        if(!fs_service_->Exists(new_file_name)) {
            file_name = new_file_name;
            break;
        }
    }

    if(file_name.empty()) {
        LOG_ERR("Failed to create log file");
        return -1;
    }

    if(!fs_service_->WriteFile(file_name, &log_metadata, sizeof(log_metadata))) {
        LOG_ERR("Failed to create log file");
        return -1;
    }

    LOG_INF("Log file created: %s", file_name.c_str());

    task_->file_name = file_name;

    atomic_set(&logger_running_, 1);

    return 0;
}

int LogWriterService::LogWriterStop() {
    if(!atomic_get(&logger_running_))
        return 0;

    atomic_set(&logger_running_, 0);

    return 0;
}

void LogWriterService::LogReadingWorkTask(k_work* work) {
    LogWriterTask* task = CONTAINER_OF(work, LogWriterTask, work);

    if(k_sem_take(task->processing_semaphore, SEMAPHORE_TIMEOUT) != 0) {
        LOG_ERR("Log writer lock timed out for sensor: %s", task->reading->sensor->id.c_str());

        return;
    }

    uint32_t value = 0;
    float float_value = task->reading->value.value_or(0.0f);
    memcpy(&value, &float_value, sizeof(value));

    auto log_record = LogRecord<uint32_t>::Create(
        TimeHelpers::ToUint32(task->reading->timestamp.value()),
        task->reading->sensor->id_hash,
        value);

    if(!task->fs_service->WriteFile(task->file_name, &log_record, sizeof(log_record), true))
        LOG_ERR("Failed to write log record");

    k_sem_give(task->processing_semaphore);
}

} // namespace eerie_leap::domain::logging_domain::services
