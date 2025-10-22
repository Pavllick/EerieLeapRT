#pragma once

#include <memory>
#include <string>
#include <chrono>

#include <zephyr/kernel.h>

#include "subsys/time/i_time_service.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/logging_domain/loggers/i_logger.h"

namespace eerie_leap::domain::logging_domain::services {

using namespace eerie_leap::subsys::time;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::logging_domain::loggers;

struct LogWriterTask {
    k_work work;
    k_sem* processing_semaphore;
    std::shared_ptr<ITimeService> time_service;
    std::shared_ptr<ILogger<SensorReading>> logger;
    std::shared_ptr<SensorReading> reading;
    system_clock::time_point start_time;
};

} // namespace eerie_leap::domain::logging_domain::services
