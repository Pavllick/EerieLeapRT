#include <memory>
#include <stdexcept>

#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/models/reading_metadata.h"

#include "sensor_processor.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities::voltage_interpolator;

SensorProcessor::SensorProcessor(std::shared_ptr<SensorReadingsFrame> sensor_readings_frame) :
    sensor_readings_frame_(std::move(sensor_readings_frame)) {}

void SensorProcessor::ProcessReading(std::shared_ptr<SensorReading> reading) {
    try {
        if(reading->status != ReadingStatus::RAW && reading->status != ReadingStatus::UNINITIALIZED)
            throw std::runtime_error("Reading is not in raw state");

        if(reading->sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
            float voltage = reading->value.value();
            float raw_value = reading->sensor->configuration.voltage_interpolator->Interpolate(voltage, true);
            float value = raw_value;

            reading->metadata.AddTag<float>(ReadingMetadataTag::RAW_VALUE, raw_value);

            if(reading->sensor->configuration.expression_evaluator != nullptr)
                value = reading->sensor->configuration.expression_evaluator->Evaluate(value);

            reading->value = value;
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::VIRTUAL_ANALOG) {
            reading->value = reading->sensor->configuration.expression_evaluator->Evaluate();
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::PHYSICAL_INDICATOR) {
            bool raw_value = reading->value.value();
            bool value = raw_value;

            reading->metadata.AddTag<bool>(ReadingMetadataTag::RAW_VALUE, raw_value > 0);

            if(reading->sensor->configuration.expression_evaluator != nullptr)
                value = reading->sensor->configuration.expression_evaluator->Evaluate(value);

            reading->value = value;
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::VIRTUAL_INDICATOR) {
            reading->value = reading->sensor->configuration.expression_evaluator->Evaluate();
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::CANBUS_RAW) {
            // No processing needed
        } else if(reading->sensor->configuration.type == SensorType::CANBUS_ANALOG) {
            float raw_value = reading->value.value();
            float value = raw_value;

            reading->metadata.AddTag<float>(ReadingMetadataTag::RAW_VALUE, raw_value);

            if(reading->sensor->configuration.expression_evaluator != nullptr)
                value = reading->sensor->configuration.expression_evaluator->Evaluate(value);

            reading->value = value;
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::CANBUS_INDICATOR) {
            bool raw_value = reading->value.value();
            bool value = raw_value;

            reading->metadata.AddTag<bool>(ReadingMetadataTag::RAW_VALUE, raw_value > 0);

            if(reading->sensor->configuration.expression_evaluator != nullptr)
                value = reading->sensor->configuration.expression_evaluator->Evaluate(value);

            reading->value = value;
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::USER_ANALOG ||
            reading->sensor->configuration.type == SensorType::USER_INDICATOR) {

            if(reading->sensor->configuration.expression_evaluator != nullptr)
                reading->value = reading->sensor->configuration.expression_evaluator->Evaluate(reading->value);

            reading->status = ReadingStatus::PROCESSED;
        } else {
            throw std::runtime_error("Unsupported sensor type");
        }
    } catch (const std::exception& e) {
        reading->status = ReadingStatus::ERROR;
        reading->error_message = e.what();
    }

    sensor_readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors
