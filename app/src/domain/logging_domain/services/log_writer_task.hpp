#pragma once

#include <memory>
#include <string>

#include <zephyr/kernel.h>

#include "subsys/fs/services/i_fs_service.h"
#include "utilities/time/i_time_service.h"
#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::logging_domain::services {

using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::utilities::time;
using namespace eerie_leap::domain::sensor_domain::models;

struct LogWriterTask {
    k_work work;
    k_sem* processing_semaphore;
    std::shared_ptr<IFsService> fs_service;
    std::shared_ptr<ITimeService> time_service;
    std::shared_ptr<SensorReading> reading;
    std::string file_name;
};

} // namespace eerie_leap::domain::logging_domain::services
