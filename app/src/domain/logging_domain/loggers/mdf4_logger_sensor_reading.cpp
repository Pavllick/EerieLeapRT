#include <cstdint>
#include <chrono>

#include "utilities/time/time_helpers.hpp"
#include "subsys/mdf/mdf4/source_information_block.h"

#include "mdf4_logger_sensor_reading.h"

namespace eerie_leap::domain::logging_domain::loggers {

using namespace eerie_leap::utilities::time;

Mdf4LoggerSensorReading::Mdf4LoggerSensorReading(std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> sensors)
    : sensors_(std::move(sensors)), stream_(nullptr) {

    mdf4_file_ = std::make_unique<Mdf4File>(false);
    auto data_group = mdf4_file_->CreateDataGroup(RECORD_ID_SIZE);

    for(auto& sensor : *sensors_) {
        auto channel_group = mdf4_file_->CreateChannelGroup(*data_group, sensor->id_hash);
        auto source_information = std::make_shared<mdf4::SourceInformationBlock>(
            mdf4::SourceInformationBlock::SourceType::IoDevice,
            mdf4::SourceInformationBlock::BusType::None,
            "Eerie Leap Sensor");
        channel_group->AddSourceInformation(source_information);

        mdf4_file_->CreateDataChannel(*channel_group, MdfDataType::Float32, sensor->id, sensor->metadata.unit);
        records_.emplace(
            sensor->id_hash,
            std::make_unique<DataRecord>(mdf4_file_->CreateDataRecord(channel_group)));
    }
}

const char* Mdf4LoggerSensorReading::GetFileExtension() const {
    return Mdf4File::LOG_DATA_FILE_EXTENSION;
}

bool Mdf4LoggerSensorReading::StartLogging(std::streambuf& stream, const system_clock::time_point& start_time) {
    stream_ = &stream;

    mdf4_file_->UpdateCurrentTime(start_time);
    mdf4_file_->WriteFileToStream(*stream_);

    return true;
}

bool Mdf4LoggerSensorReading::StopLogging() {
    stream_ = nullptr;

    return true;
}

bool Mdf4LoggerSensorReading::LogReading(const system_clock::time_point& time, const SensorReading& reading) {
    if(stream_ == nullptr)
        return false;

    if(records_.find(reading.sensor->id_hash) == records_.end() || !reading.value.has_value())
        return false;

    auto record = records_[reading.sensor->id_hash].get();
    uint32_t time_ms = TimeHelpers::ToUint32(time);
    float value = reading.value.value();
    record->WriteToStream(*stream_, {&time_ms, &value});

    return true;
}

} // namespace eerie_leap::domain::logging_domain::loggers
