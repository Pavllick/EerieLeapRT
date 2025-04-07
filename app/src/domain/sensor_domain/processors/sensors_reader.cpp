#include <stdexcept>
#include <memory>

#include "sensors_reader.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;

void SensorsReader::ReadSensors(std::vector<Sensor>& sensors) {
    for (auto& sensor : sensors) {
        auto sensor_reading = std::make_shared<SensorReading>(sensor);
        // sensor_reading.id = ;
        // sensor_reading.timestamp = time(nullptr);

        if (sensor.configuration.type == SensorType::PHYSICAL_ANALOG) {
            // Read the physical sensor
            auto reading = adc_.ReadChannel(sensor.configuration.channel.value());
            sensor_reading->value = reading;
            sensor_reading->status = ReadingStatus::RAW;

            sensor_readings_frame_.AddOrUpdateReading(sensor_reading);
        } else if (sensor.configuration.type == SensorType::VIRTUAL_ANALOG) {
            sensor_reading->status = ReadingStatus::UNINITIALIZED;
        } else {
            throw std::runtime_error("Unsupported sensor type");
        }

        sensor_readings_frame_.AddOrUpdateReading(sensor_reading);
    }
    
}
    
} // namespace eerie_leap::domain::sensor_domain::processors