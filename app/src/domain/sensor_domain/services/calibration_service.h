#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "controllers/adc_configuration_controller.h"
#include "domain/sensor_domain/services/processing_scheduler_serivce.h"

#include "sensor_task.hpp"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::controllers;

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::domain::hardware::adc_domain;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::services;

class CalibrationService {
private:
    k_sem processing_semaphore_;
    static constexpr k_timeout_t PROCESSING_TIMEOUT = K_MSEC(200);

    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<IAdcManager> adc_manager_;
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller_;
    std::shared_ptr<SensorTask> calibration_task_;
    std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service_;

    std::shared_ptr<SensorTask> CreateCalibrationTask(int channel);
    static void ProcessCalibrationWorkTask(k_work* work);

public:
    CalibrationService(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<IAdcManager> adc_manager,
        std::shared_ptr<AdcConfigurationController> adc_configuration_controller,
        std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service);

    void Start(int channel);
    void Stop();
};

} // namespace eerie_leap::domain::sensor_domain::services
