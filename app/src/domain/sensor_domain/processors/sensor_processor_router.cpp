// #include "sensor_processor_router.h"

// namespace eerie_leap::domain::sensor_domain::processors {

// SensorProcessorRouter::SensorProcessorRouter(ISensorProcessor& physical_processor, ISensorProcessor& virtual_processor)
//     : physical_processor(physical_processor), virtual_processor(virtual_processor) {}

// void SensorProcessorRouter::ProcessSensorReading(const SensorReading& reading) {
//     switch (reading.sensor->configuration.type) {
//         case SensorType::PHISICAL:
//             physical_processor.ProcessSensorReading(reading);
//             break;
//         case SensorType::VIRTUAL:
//             virtual_processor.ProcessSensorReading(reading);
//             break;
//         default:
//             break;
//     }
// }

// } // namespace eerie_leap::domain::sensor_domain::processors