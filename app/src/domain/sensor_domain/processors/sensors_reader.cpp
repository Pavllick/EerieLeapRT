#include <stdexcept>

#include "sensors_reader.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;

void SensorsReader::ReadSensors(std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> sensors) {
    for (const auto& sensor : *sensors) {
        auto sensor_reading = std::make_shared<SensorReading>(guid_generator_->Generate(), sensor);
        sensor_reading->timestamp = time_service_->GetCurrentTime();

        if (sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
            double reading = adc_->ReadChannel(sensor->configuration.channel.value());
            sensor_reading->value = reading;
            sensor_reading->status = ReadingStatus::RAW;

            sensor_readings_frame_->AddOrUpdateReading(sensor_reading);
        } else if (sensor->configuration.type == SensorType::VIRTUAL_ANALOG) {
            sensor_reading->status = ReadingStatus::UNINITIALIZED;
        } else {
            throw std::runtime_error("Unsupported sensor type");
        }

        sensor_readings_frame_->AddOrUpdateReading(sensor_reading);
    }
    
}

} // namespace eerie_leap::domain::sensor_domain::processors