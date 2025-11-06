#include <cstdint>

#include "subsys/time/time_helpers.hpp"
#include "subsys/mdf/mdf4/source_information_block.h"
#include "subsys/mdf/mdf4/channel_conversion_block.h"

#include "mdf4_logger_sensor_reading.h"

namespace eerie_leap::domain::logging_domain::loggers {

using namespace eerie_leap::subsys::time;

Mdf4LoggerSensorReading::Mdf4LoggerSensorReading(const std::vector<std::shared_ptr<Sensor>>& sensors)
    : stream_(nullptr), last_can_reading_time_(system_clock::time_point::min()) {

    mdf4_file_ = std::make_unique<Mdf4File>(false);
    auto data_group = mdf4_file_->CreateDataGroup(RECORD_ID_SIZE);

    auto source_information = std::make_shared<mdf4::SourceInformationBlock>(
        mdf4::SourceInformationBlock::SourceType::IoDevice,
        mdf4::SourceInformationBlock::BusType::None);
    source_information->SetName(mdf4_file_->GetOrCreateTextBlock("Eerie Leap Sensor"));

    for(auto& sensor : sensors) {
        if(sensor->configuration.type == SensorType::CANBUS_RAW) {
            auto vlsd_channel_group = mdf4_file_->CreateVLSDChannelGroup(data_group, vlsd_channel_group_id_++);
            auto channel_group = mdf4_file_->CreateCanDataFrameChannelGroup(data_group, vlsd_channel_group, sensor->id_hash, "Raw CAN Frame");

            can_raw_channel_groups_.emplace(sensor->id_hash, channel_group);
        } else {
            auto channel_group = mdf4_file_->CreateChannelGroup(data_group, sensor->id_hash, sensor->id);
            channel_group->AddSourceInformation(source_information);

            mdf4_file_->CreateDataChannel(channel_group, MdfDataType::Float32, "value", sensor->metadata.unit);

            if(sensor->configuration.expression_evaluator != nullptr
                && (sensor->configuration.type == SensorType::PHYSICAL_ANALOG
                    || sensor->configuration.type == SensorType::PHYSICAL_INDICATOR)) {

                auto conversion = mdf4::ChannelConversionBlock::CreateAlgebraicConversion(sensor->configuration.expression_evaluator->GetExpression());

                auto channel_raw = mdf4_file_->CreateDataChannel(channel_group, MdfDataType::Float32, "raw_value", "");
                channel_raw->SetConversion(std::make_shared<mdf4::ChannelConversionBlock>(conversion));
            }

            records_.emplace(
                sensor->id_hash,
                std::make_unique<DataRecord>(mdf4_file_->CreateDataRecord(channel_group)));
        }
    }
}

const char* Mdf4LoggerSensorReading::GetFileExtension() const {
    return Mdf4File::LOG_DATA_FILE_EXTENSION;
}

bool Mdf4LoggerSensorReading::StartLogging(std::streambuf& stream, const system_clock::time_point& start_time) {
    stream_ = &stream;
    start_time_ = start_time;
    current_file_size_bytes_ = 0;

    mdf4_file_->UpdateCurrentTime(start_time);
    current_file_size_bytes_ = mdf4_file_->WriteFileToStream(*stream_);

    return true;
}

bool Mdf4LoggerSensorReading::StopLogging() {
    stream_ = nullptr;

    return true;
}

bool Mdf4LoggerSensorReading::LogValueReading(float time_delta_s, const SensorReading& reading) {
    if(!records_.contains(reading.sensor->id_hash) || !reading.value.has_value())
        return false;

    auto record = records_[reading.sensor->id_hash].get();
    float value = reading.value.value();

    if(reading.sensor->configuration.expression_evaluator != nullptr
        && (reading.sensor->configuration.type == SensorType::PHYSICAL_ANALOG
            || reading.sensor->configuration.type == SensorType::PHYSICAL_INDICATOR)) {

        float raw_value = 0;

        if(reading.sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
            auto raw_value_data = reading.metadata.GetTag<float>(ReadingMetadataTag::RAW_VALUE);
            if(raw_value_data.has_value())
                raw_value = raw_value_data.value();
        } else if(reading.sensor->configuration.type == SensorType::PHYSICAL_INDICATOR) {
            auto raw_value_data = reading.metadata.GetTag<bool>(ReadingMetadataTag::RAW_VALUE);
            if(raw_value_data.has_value())
                raw_value = raw_value_data.value() ? 1.0F : 0.0F;
        }

        current_file_size_bytes_ += record->WriteToStream(*stream_, {&time_delta_s, &value, &raw_value});
    } else {
        current_file_size_bytes_ += record->WriteToStream(*stream_, {&time_delta_s, &value});
    }

    return true;
}

bool Mdf4LoggerSensorReading::LogCanbusRawReading(float time_delta_s, const SensorReading& reading) {
    if(!can_raw_channel_groups_.contains(reading.sensor->id_hash))
        return false;

    auto can_frame = reading.metadata.GetTag<CanFrame>(ReadingMetadataTag::CANBUS_DATA);
    if(!can_frame.has_value())
        return false;

    if(last_can_reading_time_ == reading.timestamp.value())
        return false;

    last_can_reading_time_ = reading.timestamp.value();

    auto channel_group = can_raw_channel_groups_[reading.sensor->id_hash];
    current_file_size_bytes_ += mdf4_file_->WriteCanbusDataRecordToStream(channel_group, *stream_, can_frame.value(), time_delta_s);

    return true;
}

bool Mdf4LoggerSensorReading::LogReading(const system_clock::time_point& time, const SensorReading& reading) {
    if(stream_ == nullptr)
        return false;

    float time_delta_s = (TimeHelpers::ToUint32(time) - TimeHelpers::ToUint32(start_time_)) / 1000.0F;

    if(reading.sensor->configuration.type == SensorType::CANBUS_RAW)
        return LogCanbusRawReading(time_delta_s, reading);

    return LogValueReading(time_delta_s, reading);
}

} // namespace eerie_leap::domain::logging_domain::loggers
