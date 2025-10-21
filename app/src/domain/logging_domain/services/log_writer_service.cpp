#include <string>
#include <cstdio>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/crc.h>

#include "utilities/time/time_helpers.hpp"
#include "subsys/random/rng.h"

#include "log_writer_service.h"

namespace eerie_leap::domain::logging_domain::services {

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::subsys::random;

LOG_MODULE_REGISTER(log_writer_service_logger);

LogWriterService::LogWriterService(std::shared_ptr<IFsService> fs_service, std::shared_ptr<ITimeService> time_service)
    : fs_service_(std::move(fs_service)), time_service_(std::move(time_service)), logger_running_(ATOMIC_INIT(0)) {

    k_sem_init(&processing_semaphore_, 1, 1);
}

LogWriterService::~LogWriterService() {
    k_work_queue_stop(&work_q, K_FOREVER);
    k_thread_stack_free(stack_area_);
}

void LogWriterService::SetLogger(std::shared_ptr<ILogger<SensorReading>> logger) {
    logger_ = std::move(logger);
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
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    k_work_queue_init(&work_q);
    k_work_queue_start(&work_q, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q.thread, "log_writer_service");

    task_ = std::make_shared<LogWriterTask>();
    task_->processing_semaphore = &processing_semaphore_;
    task_->time_service = time_service_;
    k_work_init(&task_->work, LogReadingWorkTask);

    LOG_INF("Log writer service initialized.");
}

std::string LogWriterService::GetNewLogDataFileName(const system_clock::time_point& tp) {
    std::string file_name_prefix = CONFIG_EERIE_LEAP_LOG_DATA_FILE_PREFIX;
    if(file_name_prefix.length() > 0)
        file_name_prefix += '_';

    std::string path = CONFIG_EERIE_LEAP_LOG_DATA_FILES_DIR;
    path += "/";
    path += file_name_prefix;
    path += std::to_string(TimeHelpers::ToUint32(tp));
    path += "_";
    path += std::to_string(Rng::Get16());

    return path;
}

int LogWriterService::LogWriterStart() {
    if(atomic_get(&logger_running_))
        return -1;

    if(!logger_) {
        LOG_ERR("Logger is not available.");
        return -1;
    }

    if(!fs_service_->IsAvailable()) {
        LOG_ERR("SD card is not available.");
        return -1;
    }

    if(!fs_service_->Exists(CONFIG_EERIE_LEAP_LOG_DATA_FILES_DIR)) {}
        if(!fs_service_->CreateDirectory(CONFIG_EERIE_LEAP_LOG_DATA_FILES_DIR)) {
            LOG_ERR("Failed to create %s directory", CONFIG_EERIE_LEAP_LOG_DATA_FILES_DIR);
            return -1;
        }

    auto start_time = time_service_->GetCurrentTime();
    std::string file_name;
    for(int i = 0; i < 10; i++) {
        auto new_file_name = GetNewLogDataFileName(start_time) + "." + logger_->GetFileExtension();
        if(!fs_service_->Exists(new_file_name)) {
            file_name = new_file_name;
            break;
        }
    }

    if(file_name.empty()) {
        LOG_ERR("Failed to create log file name");
        return -1;
    }

    fs_stream_buf_ = std::make_unique<FsServiceStreamBuf>(fs_service_.get(), file_name);
    logger_->StartLogging(*fs_stream_buf_, start_time);

    LOG_INF("Logging started. Log file created: %s", file_name.c_str());

    task_->start_time = start_time;
    task_->logger = logger_;

    atomic_set(&logger_running_, 1);

    return 0;
}

int LogWriterService::LogWriterStop() {
    if(!atomic_get(&logger_running_))
        return 0;

    atomic_set(&logger_running_, 0);

    logger_->StopLogging();
    fs_stream_buf_->close();

    LOG_INF("Logging stopped.");

    return 0;
}

void LogWriterService::LogReadingWorkTask(k_work* work) {
    LogWriterTask* task = CONTAINER_OF(work, LogWriterTask, work);

    if(k_sem_take(task->processing_semaphore, SEMAPHORE_TIMEOUT) != 0) {
        LOG_ERR("Log writer lock timed out for sensor: %s", task->reading->sensor->id.c_str());
        return;
    }

    if(!task->logger->LogReading(task->reading->timestamp.value(), *task->reading))
        LOG_ERR("Failed to log reading for sensor: %s", task->reading->sensor->id.c_str());

    k_sem_give(task->processing_semaphore);
}

} // namespace eerie_leap::domain::logging_domain::services
