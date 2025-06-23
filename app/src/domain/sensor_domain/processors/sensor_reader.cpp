#include <stdexcept>
#include <memory>
#include <vector>

#include "utilities/memory/heap_allocator.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "domain/hardware/adc_domain/utilities/adc_calibrator.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader.h"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::hardware::adc_domain::utilities;

SensorReader::SensorReader(
    std::shared_ptr<ITimeService>& time_service,
    std::shared_ptr<GuidGenerator>& guid_generator,
    std::shared_ptr<AdcConfigurationController>& adc_configuration_controller,
    std::shared_ptr<IGpio> gpio,
    std::shared_ptr<SensorReadingsFrame>& readings_frame,
    std::shared_ptr<Sensor>& sensor)
    : time_service_(time_service),
    guid_generator_(guid_generator),
    adc_configuration_controller_(adc_configuration_controller),
    gpio_(std::move(gpio)),
    readings_frame_(readings_frame),
    sensor_(sensor) {

    if(sensor_->configuration.type == SensorType::PHYSICAL_ANALOG) {
        adc_manager_ = adc_configuration_controller_->Get();
        adc_channel_configuration_ = adc_manager_->GetChannelConfiguration(sensor_->configuration.channel.value());
        AdcChannelReader = adc_manager_->GetChannelReader(sensor_->configuration.channel.value());
    }
}

void SensorReader::Read(bool is_calibration_mode) {
    auto reading = make_shared_ext<SensorReading>(guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    if(is_calibration_mode) {
        if(sensor_->configuration.type != SensorType::PHYSICAL_ANALOG)
            throw std::runtime_error("Unsupported sensor type for calibration mode");

        float voltage = AdcChannelReader();
        float voltage_interpolated = AdcCalibrator::InterpolateToInputRange(voltage);

        reading->value = voltage_interpolated;
        reading->status = ReadingStatus::CALIBRATION;
    } else if (sensor_->configuration.type == SensorType::PHYSICAL_ANALOG) {
        float voltage = AdcChannelReader();
        float voltage_calibrated = adc_channel_configuration_->calibrator->InterpolateToCalibratedRange(voltage);

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
