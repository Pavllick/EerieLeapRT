#include <stdexcept>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader_virtual_indicator.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

SensorReaderVirtualIndicator::SensorReaderVirtualIndicator(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<Sensor> sensor)
        : SensorReaderBase(
            std::move(time_service),
            std::move(guid_generator),
            std::move(sensor_readings_frame),
            std::move(sensor)) {

    if(sensor_->configuration.type != SensorType::VIRTUAL_INDICATOR)
        throw std::runtime_error("Unsupported sensor type");
}

void SensorReaderVirtualIndicator::Read() {
    auto reading = make_shared_pmr<SensorReading>(Mrm::GetExtPmr(), guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    reading->status = ReadingStatus::UNINITIALIZED;

    sensor_readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
