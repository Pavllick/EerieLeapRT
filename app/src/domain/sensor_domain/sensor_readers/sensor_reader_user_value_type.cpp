#include <stdexcept>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader_user_value_type.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::domain::sensor_domain::models;

SensorReaderUserValueType::SensorReaderUserValueType(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<Sensor> sensor)
        : SensorReaderBase(
            std::move(time_service),
            std::move(guid_generator),
            std::move(sensor_readings_frame),
            std::move(sensor)) {

    if(sensor_->configuration.type != SensorType::USER_ANALOG && sensor_->configuration.type != SensorType::USER_INDICATOR)
        throw std::runtime_error("Unsupported sensor type");

    has_create_reading_function_ = false;

    if(sensor_->configuration.lua_script == nullptr)
        return;

    lua_getglobal(sensor_->configuration.lua_script->GetState(), "create_sensor_value");

    has_create_reading_function_ = lua_isfunction(sensor_->configuration.lua_script->GetState(), -1);
    lua_pop(sensor_->configuration.lua_script->GetState(), 1);
}

void SensorReaderUserValueType::Read() {
    auto reading = make_shared_pmr<SensorReading>(Mrm::GetExtPmr(), guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    if(!has_create_reading_function_) {
        reading->status = ReadingStatus::UNINITIALIZED;

        sensor_readings_frame_->AddOrUpdateReading(reading);
        return;
    }

    auto lua_script = sensor_->configuration.lua_script;

    lua_getglobal(lua_script->GetState(), "create_sensor_value");

    if(!lua_isfunction(lua_script->GetState(), -1)) {
        lua_pop(lua_script->GetState(), 1);
        return;
    }

    lua_pushstring(lua_script->GetState(), sensor_->id.c_str());

    if(lua_pcall(lua_script->GetState(), 1, 1, 0) != LUA_OK) {
        lua_pop(lua_script->GetState(), 1);
        throw std::runtime_error("Failed to call create_sensor_value function");
    }

    if(!lua_isnumber(lua_script->GetState(), -1)) {
        lua_pop(lua_script->GetState(), 1);
        throw std::runtime_error("create_sensor_value function didn't return a number.");
    }

    auto value = static_cast<float>(lua_tonumber(lua_script->GetState(), -1));
    lua_pop(lua_script->GetState(), 1);

    reading->value = value;
    reading->status = ReadingStatus::RAW;

    sensor_readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
