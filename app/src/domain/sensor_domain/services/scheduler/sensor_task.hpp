#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensor_reader.h"
#include "domain/sensor_domain/processors/sensor_processor.h"

namespace eerie_leap::domain::sensor_domain::services::scheduler {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::sensor_domain::utilities;

struct SensorTask {
    k_work_delayable work;
    k_sem* sensors_processing_semaphore;
    k_timeout_t sampling_rate_ms;
    std::shared_ptr<Sensor> sensor;

    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame;
    std::shared_ptr<SensorReader> sensor_reader;
    std::shared_ptr<SensorProcessor> sensor_processor;
};
    
} // namespace eerie_leap::domain::sensor_domain::services::scheduler