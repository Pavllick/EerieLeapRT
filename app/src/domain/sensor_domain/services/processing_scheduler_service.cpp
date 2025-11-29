#include <span>

#include "utilities/memory/heap_allocator.h"
#include "subsys/time/time_helpers.hpp"
#include "subsys/lua_script/lua_script.h"
#include "domain/sensor_domain/processors/sensor_processor.h"
#include "domain/sensor_domain/processors/script_processor.h"
#include "domain/script_domain/utilities/global_fuctions_registry.h"

#include "processing_scheduler_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::time;
using namespace eerie_leap::subsys::lua_script;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::script_domain::utilities;

LOG_MODULE_REGISTER(processing_scheduler_logger);

ProcessingSchedulerService::ProcessingSchedulerService(
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<SensorReaderFactory> sensor_reader_factory)
        : work_queue_load_balancer_(std::make_shared<WorkQueueLoadBalancer>()),
        sensors_configuration_manager_(std::move(sensors_configuration_manager)),
        sensor_readings_frame_(std::move(sensor_readings_frame)),
        sensor_reader_factory_(std::move(sensor_reader_factory)),
        reading_processors_(std::make_shared<std::vector<std::shared_ptr<IReadingProcessor>>>()) {

    reading_processors_->push_back(make_shared_ext<ScriptProcessor>("pre_process_sensor_value"));
    reading_processors_->push_back(make_shared_ext<SensorProcessor>(sensor_readings_frame_));
    reading_processors_->push_back(make_shared_ext<ScriptProcessor>("post_process_sensor_value"));
};

void ProcessingSchedulerService::Initialize() {
    for(int i = 0; i < 2; i++) {
        auto thread = std::make_unique<WorkQueueThread>(
            "processing_scheduler_service_" + std::to_string(i),
            thread_stack_size_,
            thread_priority_ + i % 2);
        thread->Initialize();

        work_queue_load_balancer_->AddThread(std::move(thread));
    }
}

void ProcessingSchedulerService::ProcessSensorWorkTask(k_work* work) {
    SensorTask* task = CONTAINER_OF(work, SensorTask, work);

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

    task->work_q = task->work_queue_load_balancer_->GetLeastLoadedQueue().GetWorkQueue();
    k_work_reschedule_for_queue(task->work_q, &task->work, task->sampling_rate_ms);
}

std::shared_ptr<SensorTask> ProcessingSchedulerService::CreateSensorTask(std::shared_ptr<Sensor> sensor) {
    InitializeScript(sensor);
    auto reader = sensor_reader_factory_->Create(sensor);

    if(reader == nullptr)
        return nullptr;

    if(sensor->configuration.sampling_rate_ms == 0)
        return nullptr;

    auto task = make_shared_ext<SensorTask>();
    task->work_queue_load_balancer_ = work_queue_load_balancer_;
    task->work_q = work_queue_load_balancer_->GetLeastLoadedQueue().GetWorkQueue();
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
    for(auto task : sensor_tasks_) {
        k_work_delayable* work = &task->work;
        k_work_init_delayable(work, ProcessSensorWorkTask);

        k_work_schedule_for_queue(task->work_q, work, K_NO_WAIT);
    }

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

        sensor_tasks_.push_back(task);
        LOG_INF("Created task for sensor: %s", sensor->id.c_str());
    }

    StartTasks();

    LOG_INF("Processing Scheduler Service started");
}

void ProcessingSchedulerService::Restart() {
    k_work_sync sync;

    while(sensor_tasks_.size() > 0) {
        for(int i = 0; i < sensor_tasks_.size(); i++) {
            LOG_INF("Canceling task for sensor: %s", sensor_tasks_[i]->sensor->id.c_str());
            bool res = k_work_cancel_delayable_sync(&sensor_tasks_[i]->work, &sync);
            if(!res) {
                sensor_tasks_.erase(sensor_tasks_.begin() + i);
                break;
            }
        }
    }

    LOG_INF("Processing Scheduler Service stopped");

    sensor_tasks_.clear();
    sensor_readings_frame_->ClearReadings();
    Start();
}

void ProcessingSchedulerService::Pause() {
    k_work_sync sync;
    std::vector<std::shared_ptr<SensorTask>> sensor_tasks_temp;
    for(auto task : sensor_tasks_)
        sensor_tasks_temp.push_back(task);

    while(sensor_tasks_temp.size() > 0) {
        for(int i = 0; i < sensor_tasks_temp.size(); i++) {
            LOG_INF("Canceling task for sensor: %s", sensor_tasks_temp[i]->sensor->id.c_str());
            bool res = k_work_cancel_delayable_sync(&sensor_tasks_temp[i]->work, &sync);
            if(!res) {
                sensor_tasks_temp.erase(sensor_tasks_temp.begin() + i);
                break;
            }
        }
    }

    LOG_INF("Processing Scheduler Service stopped");

    sensor_tasks_temp.clear();
}

void ProcessingSchedulerService::Resume() {
    StartTasks();
}

void ProcessingSchedulerService::InitializeScript(std::shared_ptr<Sensor> sensor) {
    auto lua_script = sensor->configuration.lua_script;

    if(lua_script == nullptr)
        return;

    GlobalFunctionsRegistry::RegisterGetSensorValue(*lua_script, *sensor_readings_frame_);
    GlobalFunctionsRegistry::RegisterUpdateSensorValue(*lua_script, *sensor_readings_frame_);
}

} // namespace eerie_leap::domain::sensor_domain::services
