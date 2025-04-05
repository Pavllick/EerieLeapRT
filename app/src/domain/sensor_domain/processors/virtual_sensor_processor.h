// #pragma once

// #include "i_sensor_processor.h"
// #include "domain/sensor_domain/utilities/sensor_readings_frame.h"

// namespace eerie_leap::domain::sensor_domain::processors {

// using namespace eerie_leap::domain::sensor_domain::utilities;

// class VirtualSensorProcessor : public ISensorProcessor {
// private:
//     SensorReadingsFrame& sensor_readings_frame_;

// public:
//     explicit VirtualSensorProcessor(SensorReadingsFrame& sensor_readings_frame) :
//         sensor_readings_frame_(sensor_readings_frame) {}
//     void ProcessSensorReading(const SensorReading& reading) override;
// };

// } // namespace eerie_leap::domain::sensor_domain::processors