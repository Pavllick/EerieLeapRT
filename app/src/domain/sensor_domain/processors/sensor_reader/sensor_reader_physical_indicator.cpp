#include <stdexcept>

#include "utilities/memory/heap_allocator.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader_physical_indicator.h"

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

using namespace eerie_leap::utilities::memory;

SensorReaderPhysicalIndicator::SensorReaderPhysicalIndicator(
    std::shared_ptr<ITimeService>& time_service,
    std::shared_ptr<GuidGenerator>& guid_generator,
    std::shared_ptr<SensorReadingsFrame>& readings_frame,
    std::shared_ptr<Sensor>& sensor,
    std::shared_ptr<IGpio> gpio)
    : SensorReaderBase(time_service, guid_generator, readings_frame, sensor), gpio_(std::move(gpio)) {

    if(sensor_->configuration.type != SensorType::PHYSICAL_INDICATOR)
        throw std::runtime_error("Unsupported sensor type");
}

void SensorReaderPhysicalIndicator::Read() {
    auto reading = make_shared_ext<SensorReading>(guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    reading->value = static_cast<float>(gpio_->ReadChannel(sensor_->configuration.channel.value()));
    reading->status = ReadingStatus::RAW;

    readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
