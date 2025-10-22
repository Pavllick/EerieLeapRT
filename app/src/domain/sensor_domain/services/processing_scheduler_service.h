#pragma once

#include <memory>
#include <vector>

#include <zephyr/kernel.h>

#include "utilities/guid/guid_generator.h"
#include "subsys/time/i_time_service.h"
#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"
#include "domain/sensor_domain/configuration/adc_configuration_manager.h"
#include "subsys/gpio/i_gpio.h"

#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/i_reading_processor.h"

#include "sensor_task.hpp"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::subsys::gpio;
using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::subsys::time;
using namespace eerie_leap::domain::sensor_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::processors;

class ProcessingSchedulerService {
private:
    k_sem processing_semaphore_;
    static constexpr k_timeout_t PROCESSING_TIMEOUT = K_MSEC(200);

    static constexpr int k_stack_size_ = 4096;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(6);

    k_thread_stack_t* stack_area_;
    k_work_q work_q;

    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<IGpio> gpio_;
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager_;
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager_;

    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;

    std::vector<std::shared_ptr<SensorTask>> sensor_tasks_;
    std::shared_ptr<std::vector<std::shared_ptr<IReadingProcessor>>> reading_processors_;

    void StartTasks();
    std::shared_ptr<SensorTask> CreateSensorTask(std::shared_ptr<Sensor> sensor);
    static void ProcessSensorWorkTask(k_work* work);

public:
    ProcessingSchedulerService(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<IGpio> gpio,
        std::shared_ptr<AdcConfigurationManager> adc_configuration_manager,
        std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame);
    ~ProcessingSchedulerService();

    void Initialize();

    void RegisterReadingProcessor(std::shared_ptr<IReadingProcessor> processor);
    void Start();
    void Restart();
    void Pause();
    void Resume();
};

} // namespace eerie_leap::domain::sensor_domain::services
