#include <stdexcept>

#include "sensor_reader.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;

void SensorReader::Read(std::shared_ptr<Sensor> sensor) {
    auto reading = std::make_shared<SensorReading>(guid_generator_->Generate(), sensor);
    reading->timestamp = time_service_->GetCurrentTime();

    if (sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
        reading->value = adc_->ReadChannel(sensor->configuration.channel.value());
        reading->status = ReadingStatus::RAW;
    } else if (sensor->configuration.type == SensorType::VIRTUAL_ANALOG) {
        reading->status = ReadingStatus::UNINITIALIZED;
    } else if (sensor->configuration.type == SensorType::PHYSICAL_INDICATOR) {
        reading->value = static_cast<float>(gpio_->ReadChannel(sensor->configuration.channel.value()));
        reading->status = ReadingStatus::RAW;
    } else if (sensor->configuration.type == SensorType::VIRTUAL_INDICATOR) {
        reading->status = ReadingStatus::UNINITIALIZED;
    } else {
        throw std::runtime_error("Unsupported sensor type");
    }

    readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors
