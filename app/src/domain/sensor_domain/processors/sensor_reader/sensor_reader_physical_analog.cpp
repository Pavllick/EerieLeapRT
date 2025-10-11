#include <stdexcept>

#include "utilities/memory/heap_allocator.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader_physical_analog.h"

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::adc::utilities;

SensorReaderPhysicalAnalog::SensorReaderPhysicalAnalog(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<SensorReadingsFrame> readings_frame,
    std::shared_ptr<Sensor> sensor,
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager)
        : SensorReaderBase(
            std::move(time_service),
            std::move(guid_generator),
            std::move(readings_frame),
            std::move(sensor)),
        adc_configuration_manager_(std::move(adc_configuration_manager)) {

    if(sensor_->configuration.type != SensorType::PHYSICAL_ANALOG)
        throw std::runtime_error("Unsupported sensor type");

    adc_manager_ = adc_configuration_manager_->Get();
    adc_channel_configuration_ = adc_manager_->GetChannelConfiguration(sensor_->configuration.channel.value());
    AdcChannelReader = adc_manager_->GetChannelReader(sensor_->configuration.channel.value());
}

void SensorReaderPhysicalAnalog::Read() {
    auto reading = make_shared_ext<SensorReading>(guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    float voltage = AdcChannelReader();
    float voltage_calibrated = adc_channel_configuration_->calibrator->InterpolateToCalibratedRange(voltage);

    reading->value = voltage_calibrated;
    reading->status = ReadingStatus::RAW;

    readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
