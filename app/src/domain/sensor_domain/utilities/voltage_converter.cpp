#include <stdexcept>

#include "voltage_converter.h"

namespace eerie_leap::domain::sensor_domain::utilities {

using namespace eerie_leap::domain::sensor_domain::models;

double VoltageConverter::ConvertVoltageToValue(const SensorConfiguration& configuration, double voltage) {
    if(configuration.type != SensorType::PHYSICAL_ANALOG)
        throw std::invalid_argument("Invalid sensor type for voltage conversion");

    if(!configuration.calibration_table.has_value())
        throw std::invalid_argument("Calibration data is missing!");

    auto calibration_table = configuration.calibration_table.value();
    double voltage_range = calibration_table[1].voltage - calibration_table[0].voltage;
    double value_range = calibration_table[1].value - calibration_table[0].value;

    return ((voltage - calibration_table[0].voltage) / voltage_range * value_range) + calibration_table[0].value;
}

} // namespace eerie_leap::domain::sensor_domain::utilities