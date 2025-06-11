#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "controllers/sensors_configuration_controller.h"
#include "domain/adc_domain/hardware/i_adc.h"
#include "domain/hardware/gpio_domain/i_gpio.h"

#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensor_reader.h"
#include "domain/sensor_domain/processors/sensor_processor.h"

#include "domain/sensor_domain/utilities/indicator_readings_frame.hpp"
#include "domain/sensor_domain/processors/indicator_reader.h"
#include "domain/sensor_domain/processors/indicator_processor.h"

#include "sensor_task.hpp"
#include "indicator_task.hpp"

namespace eerie_leap::domain::sensor_domain::services::scheduler {

using namespace eerie_leap::controllers;

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::domain::hardware::gpio_domain;
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
    std::shared_ptr<IAdc> adc_;
    std::shared_ptr<IGpio> gpio_;
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller_;

    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<SensorReader> sensor_reader_;
    std::shared_ptr<SensorProcessor> sensor_processor_;

    std::shared_ptr<IndicatorReadingsFrame> indicator_readings_frame_;
    std::shared_ptr<IndicatorReader> indicator_reader_;
    std::shared_ptr<IndicatorProcessor> indicator_processor_;

    std::shared_ptr<SensorTask> CreateSensorTask(std::shared_ptr<Sensor> sensor);
    std::shared_ptr<IndicatorTask> CreateIndicatorTask(std::shared_ptr<Sensor> sensor);
    static void ProcessSensorWorkTask(k_work* work);
    static void ProcessIndicatorWorkTask(k_work* work);

public:
    ProcessingSchedulerService(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<IAdc> adc,
        std::shared_ptr<IGpio> gpio,
        std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller);

    void Start();
};

} // namespace eerie_leap::domain::sensor_domain::services::scheduler
