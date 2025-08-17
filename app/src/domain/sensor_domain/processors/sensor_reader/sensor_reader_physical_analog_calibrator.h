#pragma once

#include "sensor_reader_physical_analog.h"

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

using namespace eerie_leap::controllers;
using namespace eerie_leap::subsys::adc;

class SensorReaderPhysicalAnalogCalibrator : public SensorReaderPhysicalAnalog {
public:
    SensorReaderPhysicalAnalogCalibrator(
        std::shared_ptr<ITimeService>& time_service,
        std::shared_ptr<GuidGenerator>& guid_generator,
        std::shared_ptr<SensorReadingsFrame>& readings_frame,
        std::shared_ptr<Sensor>& sensor,
        std::shared_ptr<AdcConfigurationController>& adc_configuration_controller);

    ~SensorReaderPhysicalAnalogCalibrator() override = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
