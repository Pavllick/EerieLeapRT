#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/indicator_readings_frame.hpp"
#include "domain/sensor_domain/processors/indicator_reader.h"
#include "domain/sensor_domain/processors/indicator_processor.h"

namespace eerie_leap::domain::sensor_domain::services::scheduler {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::sensor_domain::utilities;

struct IndicatorTask {
    k_work_delayable work;
    k_sem* processing_semaphore;
    k_timeout_t sampling_rate_ms;
    std::shared_ptr<Sensor> sensor;

    std::shared_ptr<IndicatorReadingsFrame> readings_frame;
    std::shared_ptr<IndicatorReader> reader;
    std::shared_ptr<IndicatorProcessor> processor;
};

} // namespace eerie_leap::domain::sensor_domain::services::scheduler
