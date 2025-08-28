#pragma once

#include <memory>

#include <zephyr/kernel.h>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/user_com_domain/user_com.h"

namespace eerie_leap::domain::user_com_domain::interface {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::user_com_domain;

struct ComReadingTask {
    k_work work;
    k_sem* processing_semaphore;
    std::shared_ptr<UserCom> user_com;
    uint8_t user_id;
    std::shared_ptr<SensorReading> reading;
};

} // namespace eerie_leap::domain::user_com_domain::interface
