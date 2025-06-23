#pragma once

#include <functional>
#include <memory>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "controllers/adc_configuration_controller.h"
#include "domain/hardware/adc_domain/i_adc_manager.h"
#include "domain/hardware/adc_domain/models/adc_configuration.h"
#include "domain/hardware/gpio_domain/i_gpio.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

namespace eerie_leap::domain::sensor_domain::processors {

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::controllers;
using namespace eerie_leap::domain::hardware::adc_domain;
using namespace eerie_leap::domain::hardware::gpio_domain;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorReader {
private:
    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller_;
    std::shared_ptr<IGpio> gpio_;
    std::shared_ptr<SensorReadingsFrame> readings_frame_;
    std::shared_ptr<Sensor> sensor_;

    std::shared_ptr<IAdcManager> adc_manager_;
    std::shared_ptr<AdcChannelConfiguration> adc_channel_configuration_;
    std::function<float ()> AdcChannelReader;

public:
    SensorReader(
        std::shared_ptr<ITimeService>& time_service,
        std::shared_ptr<GuidGenerator>& guid_generator,
        std::shared_ptr<AdcConfigurationController>& adc_configuration_controller,
        std::shared_ptr<IGpio> gpio,
        std::shared_ptr<SensorReadingsFrame>& readings_frame,
        std::shared_ptr<Sensor>& sensor);

    void Read(bool is_calibration_mode = false);
};

} // namespace eerie_leap::domain::sensor_domain::processors
