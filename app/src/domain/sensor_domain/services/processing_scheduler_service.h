#pragma once

#include <memory>
#include <vector>

#include <zephyr/kernel.h>

#include "subsys/lua_script/lua_script.h"
#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/sensor_readers/sensor_reader_factory.h"
#include "domain/sensor_domain/processors/i_reading_processor.h"

#include "sensor_task.hpp"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::subsys::lua_script;
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
    k_work_q work_q_;

    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<SensorReaderFactory> sensor_reader_factory_;

    std::vector<std::shared_ptr<SensorTask>> sensor_tasks_;
    std::shared_ptr<std::vector<std::shared_ptr<IReadingProcessor>>> reading_processors_;

    void StartTasks();
    std::shared_ptr<SensorTask> CreateSensorTask(std::shared_ptr<Sensor> sensor);
    static void ProcessSensorWorkTask(k_work* work);

    void InitializeLuaScript(std::shared_ptr<Sensor> sensor);
    static int LuaGetReadingValue(lua_State* state);
    static int LuaUpdateReading(lua_State* state);
    static void LuaProcessReading(lua_State* state, const std::string& function_name, const std::string& sensor_id);

public:
    ProcessingSchedulerService(
        std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<SensorReaderFactory> sensor_reader_factory);
    ~ProcessingSchedulerService();

    void Initialize();

    void RegisterReadingProcessor(std::shared_ptr<IReadingProcessor> processor);
    void Start();
    void Restart();
    void Pause();
    void Resume();
};

} // namespace eerie_leap::domain::sensor_domain::services
