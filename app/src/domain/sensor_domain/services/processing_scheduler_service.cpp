#include "lua.h"
#include "utilities/memory/heap_allocator.h"
#include "subsys/time/time_helpers.hpp"
#include "domain/sensor_domain/processors/sensor_processor.h"

#include "processing_scheduler_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::time;
using namespace eerie_leap::domain::sensor_domain::models;

LOG_MODULE_REGISTER(processing_scheduler_logger);

ProcessingSchedulerService::ProcessingSchedulerService(
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<SensorReaderFactory> sensor_reader_factory)
        : sensors_configuration_manager_(std::move(sensors_configuration_manager)),
        sensor_readings_frame_(std::move(sensor_readings_frame)),
        sensor_reader_factory_(std::move(sensor_reader_factory)),
        reading_processors_(std::make_shared<std::vector<std::shared_ptr<IReadingProcessor>>>()) {

    reading_processors_->push_back(make_shared_ext<SensorProcessor>(sensor_readings_frame_));
    k_sem_init(&processing_semaphore_, 1, 1);
};

ProcessingSchedulerService::~ProcessingSchedulerService() {
    if(stack_area_ == nullptr)
        return;

    k_work_queue_stop(&work_q_, K_FOREVER);
    k_thread_stack_free(stack_area_);
}

void ProcessingSchedulerService::Initialize() {
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
    k_work_queue_init(&work_q_);
    k_work_queue_start(&work_q_, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q_.thread, "processing_scheduler_service");
}

const std::string test_script = R"(
    state = {
        counter = 0
    }

    function create_reading(sensor_id)
        return 8.1234
    end

    function pre_process_reading(sensor_id)
        state.counter = state.counter + 1
        print("Counter: " .. state.counter)
    end

    function post_process_reading(sensor_id)
        print("Sensor ID: " .. sensor_id .. ", Reading Value: " .. get_reading_value(sensor_id))
    end
)";

void ProcessingSchedulerService::ProcessSensorWorkTask(k_work* work) {
    SensorTask* task = CONTAINER_OF(work, SensorTask, work);

    if(k_sem_take(task->processing_semaphore, PROCESSING_TIMEOUT) != 0) {
        LOG_ERR("Lock take timed out for sensor: %s", task->sensor->id.c_str());
        return;
    }

    try {
        task->reader->Read();
        LuaProcessReading(task->lua_script->GetState(), "pre_process_reading", task->sensor->id);
        auto reading = task->readings_frame->GetReading(task->sensor->id_hash);

        for(auto processor : *task->reading_processors)
            processor->ProcessReading(reading);

        LuaProcessReading(task->lua_script->GetState(), "post_process_reading", task->sensor->id);

        LOG_DBG("Sensor Reading - ID: %s, Guid: %llu, Value: %.3f, Time: %s",
            task->sensor->id.c_str(),
            reading->id.AsUint64(),
            reading->value.value_or(0.0f),
            TimeHelpers::GetFormattedString(*reading->timestamp).c_str());

        LOG_INF("%s", task->sensor->id.c_str());
    } catch (const std::exception& e) {
        LOG_DBG("Error processing sensor: %s, Error: %s", task->sensor->id.c_str(), e.what());
    }

    k_sem_give(task->processing_semaphore);
    k_work_reschedule_for_queue(task->work_q, &task->work, task->sampling_rate_ms);
}

std::shared_ptr<SensorTask> ProcessingSchedulerService::CreateSensorTask(std::shared_ptr<Sensor> sensor) {
    auto lua_script = CreateLuaScript();
    auto reader = sensor_reader_factory_->Create(sensor, lua_script);

    if(reader == nullptr)
        return nullptr;

    if(sensor->configuration.sampling_rate_ms == 0)
        return nullptr;

    auto task = make_shared_ext<SensorTask>();
    task->work_q = &work_q_;
    task->processing_semaphore = &processing_semaphore_;
    task->sampling_rate_ms = K_MSEC(sensor->configuration.sampling_rate_ms);
    task->sensor = sensor;
    task->readings_frame = sensor_readings_frame_;
    task->reading_processors = reading_processors_;
    task->lua_script = lua_script;
    task->reader = std::move(reader);

    return task;
}

void ProcessingSchedulerService::StartTasks() {
    for(auto task : sensor_tasks_) {
        k_work_delayable* work = &task->work;
        k_work_init_delayable(work, ProcessSensorWorkTask);

        k_work_schedule_for_queue(&work_q_, work, K_NO_WAIT);
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

std::shared_ptr<LuaScript> ProcessingSchedulerService::CreateLuaScript() {
    auto lua_script = std::make_shared<LuaScript>();
    lua_script->RegisterGlobalFunction("get_reading_value", &ProcessingSchedulerService::LuaGetReadingValue, this);
    lua_script->RegisterGlobalFunction("update_reading", &ProcessingSchedulerService::LuaUpdateReading, this);

    lua_script->Run(test_script);

    return lua_script;
}

int ProcessingSchedulerService::LuaGetReadingValue(lua_State* state) {
    if(lua_gettop(state) != 1)
        return luaL_error(state, "Expected 1 argument");

    const auto* this_ptr = static_cast<ProcessingSchedulerService*>(lua_touserdata(state, lua_upvalueindex(1)));

    const char* sensor_id = luaL_checkstring(state, 1);

    if(!this_ptr->sensor_readings_frame_->HasReading(sensor_id)) {
        lua_pushnil(state);
        return 1;
    }

    float result = this_ptr->sensor_readings_frame_->GetReading(sensor_id)->value.value_or(0.0f);
    lua_pushnumber(state, result);

    return 1;
}

int ProcessingSchedulerService::LuaUpdateReading(lua_State* state) {
    if(lua_gettop(state) != 2)
        return luaL_error(state, "Expected 2 arguments");

    const auto* this_ptr = static_cast<ProcessingSchedulerService*>(lua_touserdata(state, lua_upvalueindex(1)));

    const char* sensor_id = luaL_checkstring(state, 1);
    float value = luaL_checknumber(state, 2);

    if(!this_ptr->sensor_readings_frame_->HasReading(sensor_id)) {
        lua_pushboolean(state, false);
        return 1;
    }

    auto reading = this_ptr->sensor_readings_frame_->GetReading(sensor_id);
    reading->value = value;

    this_ptr->sensor_readings_frame_->AddOrUpdateReading(reading);

    lua_pushboolean(state, true);

    return 1;
}

void ProcessingSchedulerService::LuaProcessReading(lua_State* state, const std::string& function_name, const std::string& sensor_id) {
    lua_getglobal(state, function_name.c_str());

    if(!lua_isfunction(state, -1)) {
        lua_pop(state, 1);
        return;
    }

    lua_pushstring(state, sensor_id.c_str());

    if(lua_pcall(state, 1, 0, 0) != LUA_OK) {
        LOG_ERR("Lua error: %s", lua_tostring(state, -1));
        lua_pop(state, 1);
    }
}

} // namespace eerie_leap::domain::sensor_domain::services
