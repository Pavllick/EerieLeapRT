#include <stdexcept>

#include "utilities/memory/heap_allocator.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader_user_value_type.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;

SensorReaderUserValueType::SensorReaderUserValueType(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<Sensor> sensor,
    std::shared_ptr<LuaScript> lua_script)
        : SensorReaderBase(
            std::move(time_service),
            std::move(guid_generator),
            std::move(sensor_readings_frame),
            std::move(sensor)),
        lua_script_(std::move(lua_script)) {

    if(sensor_->configuration.type != SensorType::USER_ANALOG && sensor_->configuration.type != SensorType::USER_INDICATOR)
        throw std::runtime_error("Unsupported sensor type");

    lua_getglobal(lua_script_->GetState(), "create_reading");

    has_create_reading_function_ = lua_isfunction(lua_script_->GetState(), -1);
    lua_pop(lua_script_->GetState(), 1);
}

void SensorReaderUserValueType::Read() {
    auto reading = make_shared_ext<SensorReading>(guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    if(!has_create_reading_function_) {
        reading->status = ReadingStatus::UNINITIALIZED;

        sensor_readings_frame_->AddOrUpdateReading(reading);
        return;
    }

    lua_getglobal(lua_script_->GetState(), "create_reading");

    if(!lua_isfunction(lua_script_->GetState(), -1)) {
        lua_pop(lua_script_->GetState(), 1);
        return;
    }

    lua_pushstring(lua_script_->GetState(), sensor_->id.c_str());

    if(lua_pcall(lua_script_->GetState(), 1, 1, 0) != LUA_OK) {
        lua_pop(lua_script_->GetState(), 1);
        throw std::runtime_error("Failed to call create_reading function");
    }

    if(!lua_isnumber(lua_script_->GetState(), -1)) {
        lua_pop(lua_script_->GetState(), 1);
        throw std::runtime_error("create_reading function didn't return a number.");
    }

    auto value = static_cast<float>(lua_tonumber(lua_script_->GetState(), -1));
    lua_pop(lua_script_->GetState(), 1);

    reading->value = value;
    reading->status = ReadingStatus::RAW;

    sensor_readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
