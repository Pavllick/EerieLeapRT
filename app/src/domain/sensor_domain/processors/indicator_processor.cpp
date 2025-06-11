#include <stdexcept>

#include "indicator_processor.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/models/reading_metadata_tag.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

void IndicatorProcessor::ProcessReading(std::shared_ptr<IndicatorReading> reading) {
    try {
        if(reading->sensor->configuration.type == SensorType::PHYSICAL_INDICATOR) {
            bool raw_value = reading->value.value();
            bool value = raw_value;

            // if(reading->sensor->configuration.expression_evaluator != nullptr) {
            //     value = reading->sensor->configuration.expression_evaluator->Evaluate(
            //         readings_frame_->GetReadingsValues(),
            //         value);
            // }

            reading->metadata.AddTag(ReadingMetadataTag::RAW_VALUE, std::to_string(raw_value));

            reading->value = value;
            reading->status = ReadingStatus::PROCESSED;

            readings_frame_->AddOrUpdateReading(reading);
        } else if(reading->sensor->configuration.type == SensorType::VIRTUAL_INDICATOR) {
            reading->value = false;
            // reading->value = reading->sensor->configuration.expression_evaluator->Evaluate(
            //     readings_frame_->GetReadingsValues());
            reading->status = ReadingStatus::PROCESSED;

            readings_frame_->AddOrUpdateReading(reading);
        } else {
            throw std::runtime_error("Unsupported sensor type");
        }
    } catch (const std::exception& e) {
        reading->status = ReadingStatus::ERROR;
        reading->error_message = e.what();
    }
}

} // namespace eerie_leap::domain::sensor_domain::processors
