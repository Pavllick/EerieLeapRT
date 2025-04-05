#include <stdexcept>

#include "voltage_converter.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

double ConvertVoltageToRawValue(const SensorConfiguration& configuration, double voltage) {
    if(configuration.type != SensorType::PHYSICAL_ANALOG)
        throw std::invalid_argument("Invalid sensor type for voltage conversion");

    if(!configuration.calibration.has_value())
        throw std::invalid_argument("Calibration data is missing!");

    auto calibration = configuration.calibration.value();
    double voltage_range = calibration.max_voltage - calibration.min_voltage;
    double value_range = calibration.max_value - calibration.min_value;

    return ((voltage - calibration.min_voltage) / voltage_range * value_range) + calibration.min_value;
}

} // namespace eerie_leap::domain::sensor_domain::utilities