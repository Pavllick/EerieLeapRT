#include <stdexcept>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader_physical_indicator.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

SensorReaderPhysicalIndicator::SensorReaderPhysicalIndicator(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<Sensor> sensor,
    std::shared_ptr<IGpio> gpio)
        : SensorReaderBase(
            std::move(time_service),
            std::move(guid_generator),
            std::move(sensor_readings_frame),
            std::move(sensor)),
        gpio_(std::move(gpio)) {

    if(sensor_->configuration.type != SensorType::PHYSICAL_INDICATOR)
        throw std::runtime_error("Unsupported sensor type");
}

void SensorReaderPhysicalIndicator::Read() {
    auto reading = make_shared_pmr<SensorReading>(Mrm::GetExtPmr(), guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    reading->value = static_cast<float>(gpio_->ReadChannel(sensor_->configuration.channel.value()));
    reading->status = ReadingStatus::RAW;

    reading->metadata.AddTag<bool>(ReadingMetadataTag::RAW_VALUE, reading->value.value() > 0);

    sensor_readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
