#pragma once

#include <memory>
#include <chrono>
#include <span>

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include "subsys/fs/services/i_fs_service.h"
#include "subsys/fs/services/fs_service_stream_buf.h"
#include "subsys/time/i_time_service.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/logging_domain/configuration/logging_configuration_manager.h"
#include "domain/logging_domain/loggers/i_logger.h"

#include "log_writer_task.hpp"

namespace eerie_leap::domain::logging_domain::services {

using namespace std::chrono;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::time;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::logging_domain::configuration;
using namespace eerie_leap::domain::logging_domain::loggers;

class LogWriterService {
private:
    std::shared_ptr<IFsService> fs_service_;
    std::shared_ptr<LoggingConfigurationManager> logging_configuration_manager_;
    std::unique_ptr<FsServiceStreamBuf> fs_stream_buf_;
    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<ILogger<SensorReading>> logger_;

    atomic_t logger_running_;

    static constexpr int k_stack_size_ = CONFIG_EERIE_LEAP_LOG_WRITER_STACK_SIZE;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(6);

    k_thread_stack_t* stack_area_;
    k_work_q work_q_;
    std::shared_ptr<LogWriterTask> task_;

    k_sem processing_semaphore_;
    static constexpr k_timeout_t SEMAPHORE_TIMEOUT = K_MSEC(200);

    static void LogReadingWorkTask(k_work* work);
    static std::string GetNewLogDataFileName(const system_clock::time_point& tp);

public:
    LogWriterService(
        std::shared_ptr<IFsService> fs_service,
        std::shared_ptr<LoggingConfigurationManager> logging_configuration_manager,
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame);
    ~LogWriterService();

    void Initialize();
    void SetLogger(std::shared_ptr<ILogger<SensorReading>> logger);

    int LogWriterStart();
    int LogWriterStop();
};

} // namespace eerie_leap::domain::logging_domain::services
