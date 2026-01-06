#pragma once

#include <memory>
#include <optional>

#include <zephyr/kernel.h>

#include "utilities/guid/guid_generator.h"
#include "subsys/threading/work_queue_thread.h"
#include "subsys/time/i_time_service.h"
#include "domain/sensor_domain/configuration/adc_configuration_manager.h"
#include "domain/sensor_domain/services/processing_scheduler_service.h"

#include "sensor_task.hpp"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::subsys::threading;
using namespace eerie_leap::subsys::time;
using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::domain::sensor_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::services;

class CalibrationService {
private:
    static constexpr int thread_stack_size_ = 4096;
    static constexpr int thread_priority_ = 6;
   std::unique_ptr<WorkQueueThread> work_queue_thread_;
   std::optional<WorkQueueTask<SensorTask>> calibration_task_;

    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager_;
    std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service_;

    std::unique_ptr<SensorTask> CreateCalibrationTask(int channel);
    static WorkQueueTaskResult ProcessCalibrationWorkTask(SensorTask* task);

public:
    CalibrationService(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<AdcConfigurationManager> adc_configuration_manager,
        std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service);

    void Initialize();

    void Start(int channel);
    void Stop();
};

} // namespace eerie_leap::domain::sensor_domain::services
