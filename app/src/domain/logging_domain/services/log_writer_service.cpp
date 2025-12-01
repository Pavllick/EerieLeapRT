#include <string>
#include <cstdio>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/crc.h>

#include "subsys/time/time_helpers.hpp"
#include "subsys/random/rng.h"

#include "log_writer_service.h"

namespace eerie_leap::domain::logging_domain::services {

using namespace eerie_leap::subsys::time;
using namespace eerie_leap::subsys::random;

LOG_MODULE_REGISTER(log_writer_service_logger);

LogWriterService::LogWriterService(
    std::shared_ptr<IFsService> fs_service,
    std::shared_ptr<LoggingConfigurationManager> logging_configuration_manager,
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame)
        : work_queue_thread_(nullptr),
        work_queue_task_(nullptr),
        fs_service_(std::move(fs_service)),
        logging_configuration_manager_(std::move(logging_configuration_manager)),
        time_service_(std::move(time_service)),
        sensor_readings_frame_(std::move(sensor_readings_frame)),
        logger_running_(ATOMIC_INIT(0)) {}

void LogWriterService::SetLogger(std::shared_ptr<ILogger<SensorReading>> logger) {
    logger_ = std::move(logger);
}

void LogWriterService::Initialize() {
    work_queue_thread_ = std::make_unique<WorkQueueThread>(
        "log_writer_service",
        thread_stack_size_,
        thread_priority_);
    work_queue_thread_->Initialize();

    auto task = std::make_unique<LogWriterTask>();
    task->time_service = time_service_;
    task->sensor_readings_frame = sensor_readings_frame_;

    work_queue_task_ = std::make_unique<WorkQueueTask<LogWriterTask>>(
        work_queue_thread_->CreateTask(ProcessWorkTask, std::move(task)));

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

    if(!fs_service_->Exists(CONFIG_EERIE_LEAP_LOG_DATA_FILES_DIR)) {
        if(!fs_service_->CreateDirectory(CONFIG_EERIE_LEAP_LOG_DATA_FILES_DIR)) {
            LOG_ERR("Failed to create %s directory", CONFIG_EERIE_LEAP_LOG_DATA_FILES_DIR);
            return -1;
        }
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

    fs_stream_buf_ = std::make_unique<FsServiceStreamBuf>(
        fs_service_.get(),
        file_name,
        FsServiceStreamBuf::OpenMode::Append);
    logger_->StartLogging(*fs_stream_buf_, start_time);

    LOG_INF("Logging started. Log file created: %s", file_name.c_str());

    work_queue_task_->user_data->logging_interval_ms =
        K_MSEC(logging_configuration_manager_->Get()->logging_interval_ms);
    work_queue_task_->user_data->start_time = start_time;
    work_queue_task_->user_data->logger = logger_;

    atomic_set(&logger_running_, 1);
    work_queue_thread_->ScheduleTask(*work_queue_task_);

    return 0;
}

int LogWriterService::LogWriterStop() {
    if(!atomic_get(&logger_running_))
        return -1;

    work_queue_thread_->CancelTask(*work_queue_task_);
    atomic_set(&logger_running_, 0);

    logger_->StopLogging();
    fs_stream_buf_->close();

    LOG_INF("Logging stopped.");

    return 0;
}

WorkQueueTaskResult LogWriterService::ProcessWorkTask(LogWriterTask* task) {
    auto time_now = task->time_service->GetCurrentTime();
    for(const auto& [sensor_id, reading] : task->sensor_readings_frame->GetReadings()) {
        if(reading->status != ReadingStatus::PROCESSED && reading->sensor->configuration.type != SensorType::CANBUS_RAW)
            continue;

        task->logger->LogReading(time_now, *reading);
    }

    return {
        .reschedule = true,
        .delay = task->logging_interval_ms
    };
}

} // namespace eerie_leap::domain::logging_domain::services
