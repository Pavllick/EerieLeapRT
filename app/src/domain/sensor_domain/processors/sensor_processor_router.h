// #pragma once

// #include <memory>
// #include "i_sensor_processor.h"

// namespace eerie_leap::domain::sensor_domain::processors {

// using namespace eerie_leap::domain::sensor_domain::models;

// class SensorProcessorRouter : public ISensorProcessor {
// private:
//     ISensorProcessor& physical_processor;
//     ISensorProcessor& virtual_processor;
// public:
//     SensorProcessorRouter(ISensorProcessor& physical_processor, ISensorProcessor& virtual_processor);
//     void ProcessSensorReading(const SensorReading& reading) override;
// };

// } // namespace eerie_leap::domain::sensor_domain::processors