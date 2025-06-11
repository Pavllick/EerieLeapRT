#include <stdexcept>

#include "sensor_processor.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/models/reading_metadata_tag.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

void SensorProcessor::ProcessReading(std::shared_ptr<SensorReading> reading) {
    try {
        if(reading->sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
            float voltage = reading->value.value();
            float raw_value = reading->sensor->configuration.voltage_interpolator->Interpolate(voltage);
            float value = raw_value;

            if(reading->sensor->configuration.expression_evaluator != nullptr) {
                value = reading->sensor->configuration.expression_evaluator->Evaluate(
                    sensor_readings_frame_->GetReadingsValues(),
                    indicator_readings_frame_->GetReadingsValues(),
                    value);
            }

            reading->metadata.AddTag(ReadingMetadataTag::VOLTAGE, std::to_string(voltage));
            reading->metadata.AddTag(ReadingMetadataTag::RAW_VALUE, std::to_string(raw_value));

            reading->value = value;
            reading->status = ReadingStatus::PROCESSED;

            sensor_readings_frame_->AddOrUpdateReading(reading);
        } else if(reading->sensor->configuration.type == SensorType::VIRTUAL_ANALOG) {
            reading->value = reading->sensor->configuration.expression_evaluator->Evaluate(
                sensor_readings_frame_->GetReadingsValues(),
                indicator_readings_frame_->GetReadingsValues());
            reading->status = ReadingStatus::PROCESSED;

            sensor_readings_frame_->AddOrUpdateReading(reading);
        } else {
            throw std::runtime_error("Unsupported sensor type");
        }
    } catch (const std::exception& e) {
        reading->status = ReadingStatus::ERROR;
        reading->error_message = e.what();
    }
}

} // namespace eerie_leap::domain::sensor_domain::processors
