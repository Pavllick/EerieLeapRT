#include "canbus_sensor_reader.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::subsys::canbus;

CanbusSensorReader::CanbusSensorReader(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<Sensor> sensor,
    std::shared_ptr<Canbus> canbus,
    std::shared_ptr<Dbc> dbc)
        : CanbusSensorReaderRaw(
            std::move(time_service),
            std::move(guid_generator),
            std::move(sensor_readings_frame),
            std::move(sensor),
            std::move(canbus)
        ),
        dbc_(std::move(dbc)) {}

void CanbusSensorReader::Read() {
    auto reading = CreateRawReading();
    if(reading == nullptr)
        return;

    reading->value = dbc_->GetMessage(sensor_->configuration.canbus_source->frame_id)->GetSignalValue(
        sensor_->configuration.canbus_source->signal_name_hash,
        reading->metadata.GetTag<CanFrame>(ReadingMetadataTag::CANBUS_DATA).value().data.data());
    reading->status = ReadingStatus::RAW;

    sensor_readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
