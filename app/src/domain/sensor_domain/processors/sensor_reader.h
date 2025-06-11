#pragma once

#include <memory>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "domain/hardware/adc_domain/i_adc.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::domain::hardware::adc_domain;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorReader {
private:
    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<IAdc> adc_;
    std::shared_ptr<SensorReadingsFrame> readings_frame_;

public:
    SensorReader(std::shared_ptr<ITimeService> time_service, std::shared_ptr<GuidGenerator> guid_generator, std::shared_ptr<IAdc> adc, std::shared_ptr<SensorReadingsFrame> readings_frame)
        : time_service_(std::move(time_service)), guid_generator_(std::move(guid_generator)), adc_(std::move(adc)), readings_frame_(std::move(readings_frame)) {}

    void Read(std::shared_ptr<Sensor> sensor);
};

} // namespace eerie_leap::domain::sensor_domain::processors
