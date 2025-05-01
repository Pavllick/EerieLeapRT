#pragma once

#include <memory>
#include <vector>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "domain/adc_domain/hardware/i_adc.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::domain::adc_domain::hardware;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorsReader {
private:
    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<IAdc> adc_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    
public:
    SensorsReader(std::shared_ptr<ITimeService> time_service, std::shared_ptr<GuidGenerator> guid_generator, std::shared_ptr<IAdc> adc, std::shared_ptr<SensorReadingsFrame> sensor_readings_frame)
        : time_service_(std::move(time_service)), guid_generator_(std::move(guid_generator)), adc_(std::move(adc)), sensor_readings_frame_(std::move(sensor_readings_frame)) {}

    void ReadSensors(std::shared_ptr<std::vector<std::shared_ptr<Sensor>>> sensors);
};

} // namespace eerie_leap::domain::sensor_domain::processors