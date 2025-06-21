#include <stdexcept>

#include "sensor_reader.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::sensor_domain::models;

SensorReader::SensorReader(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller,
    std::shared_ptr<IGpio> gpio,
    std::shared_ptr<SensorReadingsFrame> readings_frame,
    std::shared_ptr<Sensor> sensor)
        : time_service_(std::move(time_service)),
        guid_generator_(std::move(guid_generator)),
        adc_configuration_controller_(std::move(adc_configuration_controller)),
        gpio_(std::move(gpio)),
        readings_frame_(std::move(readings_frame)),
        sensor_(std::move(sensor)) {

            if(sensor_->configuration.type == SensorType::PHYSICAL_ANALOG) {
                adc_manager_ = adc_configuration_controller_->Get();
                adc_channel_configuration_ = adc_manager_->GetChannelConfiguration(sensor_->configuration.channel.value());
                AdcChannelReader = adc_manager_->GetChannelReader(sensor_->configuration.channel.value());
            }
        }

void SensorReader::Read() {
    auto reading = std::make_shared<SensorReading>(guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    if (sensor_->configuration.type == SensorType::PHYSICAL_ANALOG) {
        float voltage = AdcChannelReader();
        float voltage_calibrated = adc_channel_configuration_->voltage_interpolator->Interpolate(voltage);

        reading->value = voltage_calibrated;
        reading->status = ReadingStatus::RAW;
    } else if (sensor_->configuration.type == SensorType::VIRTUAL_ANALOG) {
        reading->status = ReadingStatus::UNINITIALIZED;
    } else if (sensor_->configuration.type == SensorType::PHYSICAL_INDICATOR) {
        reading->value = static_cast<float>(gpio_->ReadChannel(sensor_->configuration.channel.value()));
        reading->status = ReadingStatus::RAW;
    } else if (sensor_->configuration.type == SensorType::VIRTUAL_INDICATOR) {
        reading->status = ReadingStatus::UNINITIALIZED;
    } else {
        throw std::runtime_error("Unsupported sensor type");
    }

    readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors
