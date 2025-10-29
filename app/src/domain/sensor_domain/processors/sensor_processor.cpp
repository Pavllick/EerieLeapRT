#include <memory>
#include <stdexcept>

#include "utilities/memory/heap_allocator.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/models/reading_metadata.h"

#include "sensor_processor.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities;
using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities::voltage_interpolator;

SensorProcessor::SensorProcessor(std::shared_ptr<SensorReadingsFrame> readings_frame) :
    readings_frame_(std::move(readings_frame)) {}

void SensorProcessor::ProcessReading(std::shared_ptr<SensorReading> reading) {
    try {
        if(reading->sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
            float voltage = reading->value.value();
            float raw_value = reading->sensor->configuration.voltage_interpolator->Interpolate(voltage, true);
            float value = raw_value;

            reading->metadata.AddTag<float>(ReadingMetadataTag::RAW_VALUE, raw_value);

            if(reading->sensor->configuration.expression_evaluator != nullptr) {
                value = reading->sensor->configuration.expression_evaluator->Evaluate(
                    readings_frame_->GetReadingValues(),
                    value);
            }

            reading->value = value;
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::VIRTUAL_ANALOG) {
            reading->value = reading->sensor->configuration.expression_evaluator->Evaluate(
                readings_frame_->GetReadingValues());
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::PHYSICAL_INDICATOR) {
            bool raw_value = reading->value.value();
            bool value = raw_value;

            if(reading->sensor->configuration.expression_evaluator != nullptr) {
                value = reading->sensor->configuration.expression_evaluator->Evaluate(
                    readings_frame_->GetReadingValues(),
                    value);
            }

            reading->value = value;
            reading->status = ReadingStatus::PROCESSED;
        } else if(reading->sensor->configuration.type == SensorType::VIRTUAL_INDICATOR) {
            reading->value = reading->sensor->configuration.expression_evaluator->Evaluate(
                readings_frame_->GetReadingValues());
            reading->status = ReadingStatus::PROCESSED;
        } else {
            throw std::runtime_error("Unsupported sensor type");
        }
    } catch (const std::exception& e) {
        reading->status = ReadingStatus::ERROR;
        reading->error_message = e.what();
    }

    readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors
