#pragma once

#include <memory>
#include <vector>

#include "domain/adc_domain/hardware/adc.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::domain::adc_domain::hardware;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorsReader {
private:
    Adc& adc_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    
public:
    SensorsReader(Adc& adc, std::shared_ptr<SensorReadingsFrame> sensor_readings_frame) : 
        adc_(adc), sensor_readings_frame_(std::move(sensor_readings_frame)) {}

    void ReadSensors(std::vector<Sensor>& sensors);
};

} // namespace eerie_leap::domain::sensor_domain::processors