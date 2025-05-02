#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "controllers/sensors_configuration_controller.h"
#include "domain/adc_domain/hardware/i_adc.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensor_reader.h"
#include "domain/sensor_domain/processors/sensor_processor.h"

namespace eerie_leap::domain::sensor_domain::services::scheduler {

using namespace eerie_leap::controllers;

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::domain::adc_domain::hardware;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::sensor_domain::utilities;

class ProcessingSchedulerService {
private:
    k_sem sync_semaphore_;
    k_sem sensors_processing_semaphore_;
    static constexpr k_timeout_t PROCESSING_TIMEOUT = K_MSEC(200);

    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller_;
    std::shared_ptr<IAdc> adc_;

    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<SensorReader> sensor_reader_;
    std::shared_ptr<SensorProcessor> sensor_processor_;

    static void ProcessWorkTask(k_work* work);

public:
    ProcessingSchedulerService(std::shared_ptr<ITimeService> time_service, std::shared_ptr<GuidGenerator> guid_generator, std::shared_ptr<IAdc> adc, std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller);

    void Start();
};

} // namespace eerie_leap::domain::sensor_domain::services::scheduler