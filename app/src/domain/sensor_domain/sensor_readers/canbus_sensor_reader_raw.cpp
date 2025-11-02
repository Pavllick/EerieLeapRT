#include <stdexcept>

#include "utilities/memory/heap_allocator.h"

#include "canbus_sensor_reader_raw.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::utilities::memory;
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

    canbus_->RegisterHandler(
        sensor_->configuration.canbus_source->frame_id,
        [&can_frame = can_frame_, &can_frame_lock = can_frame_lock_](const CanFrame& frame) {
            auto lock_key = k_spin_lock(&can_frame_lock);
            can_frame = frame;
            k_spin_unlock(&can_frame_lock, lock_key);
        });
}

void CanbusSensorReaderRaw::Read() {
    auto reading = make_shared_ext<SensorReading>(guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    reading->value = 0;
    reading->status = ReadingStatus::RAW;

    auto lock_key = k_spin_lock(&can_frame_lock_);
    auto can_frame = can_frame_;
    k_spin_unlock(&can_frame_lock_, lock_key);

    reading->metadata.AddTag<CanFrame>(
        ReadingMetadataTag::CANBUS_DATA,
        can_frame);

    sensor_readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
