#include <stdexcept>

#include "domain/sensor_domain/models/indicator_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "indicator_reader.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;

void IndicatorReader::Read(std::shared_ptr<Sensor> sensor) {
    auto reading = std::make_shared<IndicatorReading>(guid_generator_->Generate(), sensor);
    reading->timestamp = time_service_->GetCurrentTime();

    if (sensor->configuration.type == SensorType::PHYSICAL_INDICATOR) {
        reading->value = gpio_->ReadChannel(sensor->configuration.channel.value());
        reading->status = ReadingStatus::RAW;
    } else if (sensor->configuration.type == SensorType::VIRTUAL_INDICATOR) {
        reading->status = ReadingStatus::UNINITIALIZED;
    } else {
        throw std::runtime_error("Unsupported sensor type");
    }

    readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors
