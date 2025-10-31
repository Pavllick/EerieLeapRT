#include "domain/sensor_domain/models/sensor_type.h"
#include "domain/sensor_domain/sensor_readers/sensor_reader_physical_analog.h"
#include "domain/sensor_domain/sensor_readers/sensor_reader_physical_indicator.h"
#include "domain/sensor_domain/sensor_readers/sensor_reader_virtual_analog.h"
#include "domain/sensor_domain/sensor_readers/sensor_reader_virtual_indicator.h"
#include "domain/sensor_domain/sensor_readers/canbus_sensor_reader_raw.h"
#include "domain/sensor_domain/sensor_readers/canbus_sensor_reader.h"

#include "sensor_reader_factory.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

SensorReaderFactory::SensorReaderFactory(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<IGpio> gpio,
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
    std::shared_ptr<Canbus> canbus,
    std::shared_ptr<Dbc> dbc)
        : time_service_(std::move(time_service)),
        guid_generator_(std::move(guid_generator)),
        gpio_(std::move(gpio)),
        adc_configuration_manager_(std::move(adc_configuration_manager)),
        sensor_readings_frame_(std::move(sensor_readings_frame)),
        canbus_(std::move(canbus)),
        dbc_(std::move(dbc)) {}

std::unique_ptr<ISensorReader> SensorReaderFactory::Create(std::shared_ptr<Sensor> sensor) {
    std::unique_ptr<ISensorReader> sensor_reader;

    if(sensor->configuration.type == SensorType::PHYSICAL_ANALOG) {
        sensor_reader = make_unique<SensorReaderPhysicalAnalog>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor,
            adc_configuration_manager_);
    } else if(sensor->configuration.type == SensorType::VIRTUAL_ANALOG) {
        sensor_reader = make_unique<SensorReaderVirtualAnalog>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor);
    } else if(sensor->configuration.type == SensorType::PHYSICAL_INDICATOR) {
        sensor_reader = make_unique<SensorReaderPhysicalIndicator>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor,
            gpio_);
    } else if(sensor->configuration.type == SensorType::VIRTUAL_INDICATOR) {
        sensor_reader = make_unique<SensorReaderVirtualIndicator>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor);
    } else if(sensor->configuration.type == SensorType::CANBUS_RAW) {
        sensor_reader = make_unique<CanbusSensorReaderRaw>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor,
            canbus_);
    } else if(sensor->configuration.type == SensorType::CANBUS_ANALOG || sensor->configuration.type == SensorType::CANBUS_INDICATOR) {
        sensor_reader = make_unique<CanbusSensorReader>(
            time_service_,
            guid_generator_,
            sensor_readings_frame_,
            sensor,
            canbus_,
            dbc_);
    } else {
        throw std::runtime_error("Unsupported sensor type");
    }

    return sensor_reader;
}

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
