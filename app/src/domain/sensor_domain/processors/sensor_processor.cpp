#include <stdexcept>

#include "sensor_processor.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/models/reading_metadata_tag.h"
#include "domain/sensor_domain/utilities/voltage_converter.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

void SensorProcessor::ProcessSensorReading(SensorReading& reading) {
    if(reading.status != ReadingStatus::RAW)
        return;
    
    try {
        double voltage = reading.value.value();
        double raw_value = VoltageConverter::ConvertVoltageToValue(
            reading.sensor->configuration, voltage);
        double value = raw_value;

        if(reading.sensor->configuration.expression_evaluator != nullptr)
            value = reading.sensor->configuration.expression_evaluator->Evaluate(
                value,
                sensor_readings_frame_->GetReadingsValues());

        if(reading.sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
            reading.metadata.AddTag(ReadingMetadataTag::VOLTAGE, std::to_string(voltage));
            reading.metadata.AddTag(ReadingMetadataTag::RAW_VALUE, std::to_string(raw_value));
        }

        reading.value = value;
        reading.status = ReadingStatus::PROCESSED;
    } catch (const std::exception& e) {
        reading.status = ReadingStatus::ERROR;
        reading.error_message = e.what();
    }
}

} // namespace eerie_leap::domain::sensor_domain::processors