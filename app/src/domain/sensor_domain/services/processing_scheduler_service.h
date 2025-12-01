#pragma once

#include <memory>
#include <vector>

#include <zephyr/kernel.h>

#include "subsys/threading/work_queue_thread.h"
#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/sensor_readers/sensor_reader_factory.h"
#include "domain/sensor_domain/processors/i_reading_processor.h"

#include "sensor_task.hpp"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::subsys::threading;
using namespace eerie_leap::domain::sensor_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::processors;

class ProcessingSchedulerService {
private:
    static constexpr int thread_stack_size_ = 8192;
    static constexpr int thread_priority_ = 6;
    std::unique_ptr<WorkQueueThread> work_queue_thread_;

    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<SensorReaderFactory> sensor_reader_factory_;

    std::vector<WorkQueueTask<SensorTask>> work_queue_tasks_;
    std::shared_ptr<std::vector<std::shared_ptr<IReadingProcessor>>> reading_processors_;

    void StartTasks();
    std::unique_ptr<SensorTask> CreateSensorTask(std::shared_ptr<Sensor> sensor);
    static WorkQueueTaskResult ProcessSensorWorkTask(SensorTask* task);

    void InitializeScript(std::shared_ptr<Sensor> sensor);

public:
    ProcessingSchedulerService(
        std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<SensorReaderFactory> sensor_reader_factory);
    ~ProcessingSchedulerService() = default;

    void Initialize();

    void RegisterReadingProcessor(std::shared_ptr<IReadingProcessor> processor);
    void Start();
    void Restart();
    void Pause();
    void Resume();
};

} // namespace eerie_leap::domain::sensor_domain::services
