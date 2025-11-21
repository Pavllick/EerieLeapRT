#include "utilities/memory/heap_allocator.h"
#include "domain/canbus_domain/processors/script_processor.h"
#include "domain/script_domain/utilities/global_fuctions_registry.h"

#include "canbus_scheduler_service.h"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::script_domain::utilities;

LOG_MODULE_REGISTER(canbus_scheduler_logger);

CanbusSchedulerService::CanbusSchedulerService(
    std::shared_ptr<CanbusConfigurationManager> canbus_configuration_manager,
    std::shared_ptr<CanbusService> canbus_service,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame)
        : canbus_configuration_manager_(std::move(canbus_configuration_manager)),
        canbus_service_(std::move(canbus_service)),
        sensor_readings_frame_(std::move(sensor_readings_frame)),
        can_frame_processors_(std::make_shared<std::vector<std::shared_ptr<ICanFrameProcessor>>>()) {

    can_frame_dbc_builder_ = make_shared_ext<CanFrameDbcBuilder>(canbus_service_, sensor_readings_frame_);
    can_frame_processors_->push_back(make_shared_ext<ScriptProcessor>(sensor_readings_frame_, "process_can_frame"));

    k_sem_init(&processing_semaphore_, 1, 1);
};

CanbusSchedulerService::~CanbusSchedulerService() {
    if(stack_area_ == nullptr)
        return;

    k_work_queue_stop(&work_q_, K_FOREVER);
    k_thread_stack_free(stack_area_);
}

void CanbusSchedulerService::Initialize() {
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    k_work_queue_init(&work_q_);
    k_work_queue_start(&work_q_, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q_.thread, "canbus_scheduler_service");
}

void CanbusSchedulerService::ProcessCanbusWorkTask(k_work* work) {
    CanbusTask* task = CONTAINER_OF(work, CanbusTask, work);

    if(k_sem_take(task->processing_semaphore, PROCESSING_TIMEOUT) != 0) {
        LOG_ERR("Lock take timed out for Frame ID: %d", task->message_configuration.frame_id);
        return;
    }

    try {
        auto can_frame = task->can_frame_dbc_builder->Build(task->bus_channel, task->message_configuration.frame_id);

        for(const auto& processor : *task->can_frame_processors)
            can_frame = processor->Process(task->message_configuration, can_frame);

        if(can_frame.data.size() > 0)
            task->canbus->SendFrame(can_frame);
    } catch (const std::exception& e) {
        LOG_DBG("Error processing Frame ID: %d, Error: %s", task->message_configuration.frame_id, e.what());
    }

    k_sem_give(task->processing_semaphore);
    k_work_reschedule_for_queue(task->work_q, &task->work, task->send_interval_ms);
}

std::shared_ptr<CanbusTask> CanbusSchedulerService::CreateTask(uint8_t bus_channel, const CanMessageConfiguration& message_configuration) {
    auto canbus = canbus_service_->GetCanbus(bus_channel);
    if(canbus == nullptr) {
        LOG_ERR("Failed to create task for Frame ID: %d", message_configuration.frame_id);
        return nullptr;
    }

    auto dbc = canbus_service_->GetChannelConfiguration(bus_channel)->dbc;
    if(dbc == nullptr || !dbc->IsLoaded()) {
        LOG_ERR("Failed to create task for Frame ID: %d", message_configuration.frame_id);
        return nullptr;
    }

    if(message_configuration.send_interval_ms == 0)
        return nullptr;

    InitializeScript(message_configuration);

    auto task = make_shared_ext<CanbusTask>();
    task->work_q = &work_q_;
    task->processing_semaphore = &processing_semaphore_;
    task->send_interval_ms = K_MSEC(message_configuration.send_interval_ms);
    task->bus_channel = bus_channel;
    task->message_configuration = message_configuration;
    task->canbus = canbus;
    task->can_frame_dbc_builder = can_frame_dbc_builder_;
    task->can_frame_processors = can_frame_processors_;

    dbc->RegisterAllSignalsForFrame(task->message_configuration.frame_id);

    return task;
}

void CanbusSchedulerService::StartTasks() {
    for(auto task : tasks_) {
        k_work_delayable* work = &task->work;
        k_work_init_delayable(work, ProcessCanbusWorkTask);

        k_work_schedule_for_queue(&work_q_, work, K_NO_WAIT);
    }

    k_sleep(K_MSEC(1));
}

void CanbusSchedulerService::Start() {
    auto canbus_configuration = canbus_configuration_manager_->Get();

    for(const auto& [bus_channel, channel_configuration] : canbus_configuration->channel_configurations) {
        for(const auto& message_configuration : channel_configuration.message_configurations) {
            auto task = CreateTask(bus_channel, message_configuration);
            if(task == nullptr)
                continue;

            tasks_.push_back(task);
            LOG_INF("Created task for Frame ID: %d", message_configuration.frame_id);
        }
    }

    StartTasks();

    LOG_INF("CANBus Scheduler Service started");
}

void CanbusSchedulerService::Restart() {
    k_work_sync sync;

    while(tasks_.size() > 0) {
        for(int i = 0; i < tasks_.size(); i++) {
            LOG_INF("Canceling task for Frame ID: %d", tasks_[i]->message_configuration.frame_id);
            bool res = k_work_cancel_delayable_sync(&tasks_[i]->work, &sync);
            if(!res) {
                tasks_.erase(tasks_.begin() + i);
                break;
            }
        }
    }

    LOG_INF("CANBus Scheduler Service stopped");

    tasks_.clear();
    sensor_readings_frame_->ClearReadings();
    Start();
}

void CanbusSchedulerService::Pause() {
    k_work_sync sync;
    std::vector<std::shared_ptr<CanbusTask>> tasks_temp;
    for(auto task : tasks_)
        tasks_temp.push_back(task);

    while(tasks_temp.size() > 0) {
        for(int i = 0; i < tasks_temp.size(); i++) {
            LOG_INF("Canceling task for Frame ID: %d", tasks_temp[i]->message_configuration.frame_id);
            bool res = k_work_cancel_delayable_sync(&tasks_temp[i]->work, &sync);
            if(!res) {
                tasks_temp.erase(tasks_temp.begin() + i);
                break;
            }
        }
    }

    LOG_INF("CANBus Scheduler Service stopped");

    tasks_temp.clear();
}

void CanbusSchedulerService::Resume() {
    StartTasks();
}

void CanbusSchedulerService::InitializeScript(const CanMessageConfiguration& message_configuration) {
    auto lua_script = message_configuration.lua_script;

    if(lua_script == nullptr)
        return;

    GlobalFunctionsRegistry::RegisterGetSensorValue(*lua_script, *sensor_readings_frame_);
    GlobalFunctionsRegistry::RegisterUpdateSensorValue(*lua_script, *sensor_readings_frame_);
}

} // namespace eerie_leap::domain::canbus_domain::services
