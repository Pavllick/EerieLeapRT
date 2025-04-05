// #pragma once

// #include "i_sensor_processor.h"
// #include "domain/adc_domain/hardware/adc.h"
// #include "domain/sensor_domain/utilities/sensor_readings_frame.h"

// namespace eerie_leap::domain::sensor_domain::processors {

// using namespace eerie_leap::domain::adc_domain::hardware;
// using namespace eerie_leap::domain::sensor_domain::utilities;

// class PhysicalSensorProcessor : public ISensorProcessor {
// private:
//     Adc& adc_;
//     SensorReadingsFrame& sensor_readings_frame_;

// public:
//     PhysicalSensorProcessor(Adc& adc, SensorReadingsFrame& sensor_readings_frame) :
//         adc_(adc), sensor_readings_frame_(sensor_readings_frame) {}
//     void ProcessSensorReading(const SensorReading& reading) override;
// };

// } // namespace eerie_leap::domain::sensor_domain::processors