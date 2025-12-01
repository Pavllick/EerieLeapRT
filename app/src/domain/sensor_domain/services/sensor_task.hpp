#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "subsys/threading/work_queue_thread.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/sensor_readers/i_sensor_reader.h"
#include "domain/sensor_domain/processors/i_reading_processor.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::subsys::threading;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::sensor_domain::sensor_readers;
using namespace eerie_leap::domain::sensor_domain::utilities;

struct SensorTask {
    k_timeout_t sampling_rate_ms;
    std::shared_ptr<Sensor> sensor;

    std::shared_ptr<SensorReadingsFrame> readings_frame;
    std::unique_ptr<ISensorReader> reader;
    std::shared_ptr<std::vector<std::shared_ptr<IReadingProcessor>>> reading_processors;
};

} // namespace eerie_leap::domain::sensor_domain::services
