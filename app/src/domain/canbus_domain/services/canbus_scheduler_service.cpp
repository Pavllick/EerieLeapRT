#include "utilities/memory/heap_allocator.h"

#include "canbus_scheduler_service.h"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;

LOG_MODULE_REGISTER(canbus_scheduler_logger);

CanbusSchedulerService::CanbusSchedulerService(
    std::shared_ptr<CanbusConfigurationManager> canbus_configuration_manager,
    std::shared_ptr<CanbusService> canbus_service,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame)
        : canbus_configuration_manager_(std::move(canbus_configuration_manager)),
        canbus_service_(std::move(canbus_service)),
        sensor_readings_frame_(std::move(sensor_readings_frame)) {

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
        auto can_data = task->dbc->EncodeMessage(
            task->message_configuration.frame_id,
            [&task](size_t signal_name_hash) {
                if(!task->readings_frame->HasReading(signal_name_hash))
                    return 0.0f;

                return task->readings_frame->GetReadingValue(signal_name_hash);
            });

        CanFrame can_frame = {
            .id = task->message_configuration.frame_id,
            .is_transmit = true,
            .data = can_data
        };
        task->canbus->SendFrame(can_frame);
    } catch (const std::exception& e) {
        LOG_DBG("Error processing Frame ID: %d, Error: %s", task->message_configuration.frame_id, e.what());
    }

    k_sem_give(task->processing_semaphore);
    k_work_reschedule_for_queue(task->work_q, &task->work, task->send_interval_ms);
}

std::shared_ptr<CanbusTask> CanbusSchedulerService::CreateTask(uint8_t bus_channel, const CanMessageConfiguration& message_configuration) {
    auto task = make_shared_ext<CanbusTask>();
    task->work_q = &work_q_;
    task->processing_semaphore = &processing_semaphore_;
    task->send_interval_ms = K_MSEC(message_configuration.send_interval_ms);
    task->canbus = canbus_service_->GetCanbus(bus_channel);
    task->dbc = canbus_service_->GetDbcForChannel(bus_channel);
    task->readings_frame = sensor_readings_frame_;
    task->message_configuration = message_configuration;

    if(task->canbus == nullptr || task->dbc == nullptr || !task->dbc->IsLoaded()) {
        LOG_ERR("Failed to create task for Frame ID: %d", message_configuration.frame_id);
        return nullptr;
    }

    task->dbc->RegisterAllSignalsForFrame(task->message_configuration.frame_id);

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

    for(const auto& channel_configuration : canbus_configuration->channel_configurations) {
        for(const auto& message_configuration : channel_configuration.message_configurations) {
            auto task = CreateTask(channel_configuration.bus_channel, message_configuration);
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

} // namespace eerie_leap::domain::canbus_domain::services
