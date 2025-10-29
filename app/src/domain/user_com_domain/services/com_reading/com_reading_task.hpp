#pragma once

#include <memory>

#include <zephyr/kernel.h>

#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/user_com_domain/user_com.h"

namespace eerie_leap::domain::user_com_domain::services::com_reading {

using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::user_com_domain;

struct ComReadingTask {
    k_work_q* work_q;
    k_work_delayable work;
    k_sem* processing_semaphore;
    int sending_interval_ms;
    uint8_t user_id;
    std::shared_ptr<UserCom> user_com;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame;
};

} // namespace eerie_leap::domain::user_com_domain::services::com_reading
