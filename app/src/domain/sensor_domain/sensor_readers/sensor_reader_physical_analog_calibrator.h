#pragma once

#include "sensor_reader_physical_analog.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::domain::sensor_domain::configuration;

class SensorReaderPhysicalAnalogCalibrator : public SensorReaderPhysicalAnalog {
public:
    SensorReaderPhysicalAnalogCalibrator(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<Sensor> sensor,
        std::shared_ptr<AdcConfigurationManager> adc_configuration_manager);

    ~SensorReaderPhysicalAnalogCalibrator() override = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
