#include <stdexcept>

#include "canbus_sensor_reader_raw.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::subsys::canbus;

CanbusSensorReaderRaw::CanbusSensorReaderRaw(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<Sensor> sensor,
    std::shared_ptr<Canbus> canbus)
        : SensorReaderBase(
            std::move(time_service),
            std::move(guid_generator),
            std::move(sensor_readings_frame),
            std::move(sensor)),
        canbus_(std::move(canbus)) {

    canbus_->RegisterFrameReceivedHandler(
        sensor_->configuration.canbus_source->frame_id,
        [this](const CanFrame& frame) {
            auto lock_key = k_spin_lock(&can_frame_lock_);
            can_frame_ = frame;
            can_frame_timestamp_ = time_service_->GetCurrentTime();
            k_spin_unlock(&can_frame_lock_, lock_key);
        });
}

std::shared_ptr<SensorReading> CanbusSensorReaderRaw::CreateRawReading() {
    if(can_frame_.data.empty())
        return nullptr;

    auto reading = std::make_shared<SensorReading>(guid_generator_->Generate(), sensor_);

    reading->value = std::nullopt;
    reading->status = ReadingStatus::RAW;

    auto lock_key = k_spin_lock(&can_frame_lock_);
    auto can_frame = can_frame_;
    reading->timestamp = can_frame_timestamp_;
    k_spin_unlock(&can_frame_lock_, lock_key);

    reading->metadata.AddTag<CanFrame>(
        ReadingMetadataTag::CANBUS_DATA,
        can_frame);

    return reading;
}

void CanbusSensorReaderRaw::Read() {
    auto reading = CreateRawReading();
    if(reading == nullptr)
        return;

    sensor_readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
