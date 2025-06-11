#pragma once

#include <memory>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "domain/hardware/gpio_domain/i_gpio.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/indicator_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::domain::hardware::gpio_domain;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

class IndicatorReader {
private:
    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<IGpio> gpio_;
    std::shared_ptr<IndicatorReadingsFrame> readings_frame_;

public:
    IndicatorReader(std::shared_ptr<ITimeService> time_service, std::shared_ptr<GuidGenerator> guid_generator, std::shared_ptr<IGpio> gpio, std::shared_ptr<IndicatorReadingsFrame> readings_frame)
        : time_service_(std::move(time_service)), guid_generator_(std::move(guid_generator)), gpio_(std::move(gpio)), readings_frame_(std::move(readings_frame)) {}

    void Read(std::shared_ptr<Sensor> sensor);
};

} // namespace eerie_leap::domain::sensor_domain::processors
