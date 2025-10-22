#include <stdexcept>

#include "utilities/memory/heap_allocator.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader_physical_analog_calibrator.h"

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::adc::utilities;

SensorReaderPhysicalAnalogCalibrator::SensorReaderPhysicalAnalogCalibrator(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<SensorReadingsFrame> readings_frame,
    std::shared_ptr<Sensor> sensor,
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager)
        : SensorReaderPhysicalAnalog(
            std::move(time_service),
            std::move(guid_generator),
            std::move(readings_frame),
            std::move(sensor),
            std::move(adc_configuration_manager)) { }

void SensorReaderPhysicalAnalogCalibrator::Read() {
    auto reading = make_shared_ext<SensorReading>(guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    float voltage = AdcChannelReader();
    float voltage_interpolated = AdcCalibrator::InterpolateToInputRange(voltage);

    reading->value = voltage_interpolated;
    reading->status = ReadingStatus::CALIBRATION;

    reading->metadata.AddTag<float>(ReadingMetadataTag::VOLTAGE, voltage);
    reading->metadata.AddTag<float>(ReadingMetadataTag::RAW_VALUE, voltage_interpolated);

    readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
