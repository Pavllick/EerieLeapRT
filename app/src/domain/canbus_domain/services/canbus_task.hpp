#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "subsys/canbus/canbus.h"
#include "subsys/dbc/dbc.h"
#include "domain/canbus_domain/models/can_message_configuration.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::subsys::dbc;
using namespace eerie_leap::domain::canbus_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

struct CanbusTask {
    k_work_q* work_q;
    k_work_delayable work;
    k_sem* processing_semaphore;
    k_timeout_t send_interval_ms;
    std::shared_ptr<Canbus> canbus;
    std::shared_ptr<Dbc> dbc;
    std::shared_ptr<SensorReadingsFrame> readings_frame;
    CanMessageConfiguration message_configuration;
};

} // namespace eerie_leap::domain::canbus_domain::services
