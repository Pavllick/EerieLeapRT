#include <span>

#include "subsys/time/time_helpers.hpp"
#include "subsys/lua_script/lua_script.h"
#include "domain/sensor_domain/processors/sensor_processor.h"
#include "domain/sensor_domain/processors/script_processor.h"
#include "domain/script_domain/utilities/global_fuctions_registry.h"

#include "processing_scheduler_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::subsys::time;
using namespace eerie_leap::subsys::lua_script;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::script_domain::utilities;

LOG_MODULE_REGISTER(processing_scheduler_logger);

ProcessingSchedulerService::ProcessingSchedulerService(
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<SensorReaderFactory> sensor_reader_factory)
        : work_queue_thread_(nullptr),
        sensors_configuration_manager_(std::move(sensors_configuration_manager)),
        sensor_readings_frame_(std::move(sensor_readings_frame)),
        sensor_reader_factory_(std::move(sensor_reader_factory)),
        reading_processors_(std::make_shared<std::vector<std::shared_ptr<IReadingProcessor>>>()) {

    reading_processors_->push_back(std::make_shared<ScriptProcessor>("pre_process_sensor_value"));
    reading_processors_->push_back(std::make_shared<SensorProcessor>(sensor_readings_frame_));
    reading_processors_->push_back(std::make_shared<ScriptProcessor>("post_process_sensor_value"));
};

void ProcessingSchedulerService::Initialize() {
    work_queue_thread_ = std::make_unique<WorkQueueThread>(
        "processing_scheduler_service",
        thread_stack_size_,
        thread_priority_);
    work_queue_thread_->Initialize();
}

WorkQueueTaskResult ProcessingSchedulerService::ProcessSensorWorkTask(SensorTask* task) {
    try {
        task->reader->Read();
        auto reading = task->readings_frame->GetReading(task->sensor->id_hash);

        for(auto processor : *task->reading_processors)
            processor->ProcessReading(reading);

        LOG_DBG("Sensor Reading - ID: %s, Guid: %llu, Value: %.3f, Time: %s",
            task->sensor->id.c_str(),
            reading->id.AsUint64(),
            reading->value.value_or(0.0f),
            TimeHelpers::GetFormattedString(*reading->timestamp).c_str());
    } catch (const std::exception& e) {
        LOG_DBG("Error processing sensor: %s, Error: %s", task->sensor->id.c_str(), e.what());
    }

    return {
        .reschedule = true,
        .delay = task->sampling_rate_ms
    };
}

std::unique_ptr<SensorTask> ProcessingSchedulerService::CreateSensorTask(std::shared_ptr<Sensor> sensor) {
    InitializeScript(sensor);
    auto reader = sensor_reader_factory_->Create(sensor);

    if(reader == nullptr)
        return nullptr;

    if(sensor->configuration.sampling_rate_ms == 0)
        return nullptr;

    auto task = std::make_unique<SensorTask>();
    task->sampling_rate_ms = K_MSEC(sensor->configuration.sampling_rate_ms);
    task->sensor = sensor;
    task->readings_frame = sensor_readings_frame_;
    task->reading_processors = reading_processors_;
    task->reader = std::move(reader);

    if(sensor->configuration.expression_evaluator != nullptr) {
        sensor->configuration.expression_evaluator->RegisterVariableValueHandler(
            [&sensor_readings_frame = sensor_readings_frame_](const std::string& sensor_id) {
                return sensor_readings_frame->GetReadingValuePtr(sensor_id);
            });
    }

    return task;
}

void ProcessingSchedulerService::StartTasks() {
    for(auto& work_queue_task : work_queue_tasks_)
        work_queue_task.Schedule();

    k_sleep(K_MSEC(1));
}

void ProcessingSchedulerService::RegisterReadingProcessor(std::shared_ptr<IReadingProcessor> processor) {
    reading_processors_->push_back(processor);
}

void ProcessingSchedulerService::Start() {
    const auto* sensors = sensors_configuration_manager_->Get();

    for(const auto& sensor : *sensors) {
        auto task = CreateSensorTask(sensor);
        if(task == nullptr)
            continue;

        work_queue_tasks_.emplace_back(
            work_queue_thread_->CreateTask(ProcessSensorWorkTask, std::move(task)));
        LOG_INF("Created task for sensor: %s", sensor->id.c_str());
    }

    StartTasks();

    LOG_INF("Processing Scheduler Service started.");
}

void ProcessingSchedulerService::Restart() {
    Pause();
    work_queue_tasks_.clear();
    sensor_readings_frame_->ClearReadings();
    Start();
}

void ProcessingSchedulerService::Pause() {
    for(auto& work_queue_task : work_queue_tasks_) {
        LOG_INF("Canceling task for sensor: %s", work_queue_task.GetUserdata()->sensor->id.c_str());

        while(work_queue_task.Cancel())
            k_sleep(K_MSEC(1));
    }

    LOG_INF("Processing Scheduler Service stopped.");
}

void ProcessingSchedulerService::Resume() {
    for(auto& work_queue_task : work_queue_tasks_)
        work_queue_task.Schedule();

    LOG_INF("Processing Scheduler Service resumed.");
}

void ProcessingSchedulerService::InitializeScript(std::shared_ptr<Sensor> sensor) {
    auto lua_script = sensor->configuration.lua_script;

    if(lua_script == nullptr)
        return;

    GlobalFunctionsRegistry::RegisterGetSensorValue(*lua_script, *sensor_readings_frame_);
    GlobalFunctionsRegistry::RegisterUpdateSensorValue(*lua_script, *sensor_readings_frame_);
}

} // namespace eerie_leap::domain::sensor_domain::services
