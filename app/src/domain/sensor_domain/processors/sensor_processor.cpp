#include <stdexcept>

#include "sensor_processor.h"
#include "utilities/expression_evaluator.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/models/reading_metadata_tag.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities;
using namespace eerie_leap::domain::sensor_domain::models;

void SensorProcessor::ProcessSensorReading(SensorReading& reading) {
    if(reading.status != ReadingStatus::RAW)
        return;
    
    try {
        double raw_value = 0.0;
        double value = raw_value;

        if(reading.sensor->configuration.conversion_expression_raw.has_value())
            value = ExpressionEvaluator::Evaluate(
                reading.sensor->configuration.conversion_expression_raw.value(),
                raw_value,
                sensor_readings_frame_.GetAllReadings());

        if(reading.sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
            reading.metadata.AddTag(ReadingMetadataTag::VOLTAGE, std::to_string(reading.value.value()));
            reading.metadata.AddTag(ReadingMetadataTag::RAW_VALUE, std::to_string(raw_value));
        }

        reading.value = value;
        reading.status = ReadingStatus::PROCESSED;
        
        sensor_readings_frame_.AddReading(reading.id.value(), value);
    } catch (const std::exception& e) {
        reading.status = ReadingStatus::ERROR;
        reading.error_message = e.what();
    }
}

} // namespace eerie_leap::domain::sensor_domain::processors