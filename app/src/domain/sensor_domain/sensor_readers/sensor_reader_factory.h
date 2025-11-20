#pragma once

#include <memory>

#include "utilities/guid/guid_generator.h"
#include "subsys/time/i_time_service.h"
#include "subsys/gpio/i_gpio.h"

#include "domain/sensor_domain/configuration/adc_configuration_manager.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/canbus_domain/services/canbus_service.h"
#include "i_sensor_reader.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::subsys::time;
using namespace eerie_leap::subsys::gpio;

using namespace eerie_leap::domain::sensor_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::canbus_domain::services;

class SensorReaderFactory {
protected:
    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<IGpio> gpio_;
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<CanbusService> canbus_service_;

public:
    SensorReaderFactory(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<IGpio> gpio,
        std::shared_ptr<AdcConfigurationManager> adc_configuration_manager,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<CanbusService> canbus_service);

    virtual ~SensorReaderFactory() = default;

    std::unique_ptr<ISensorReader> Create(std::shared_ptr<Sensor> sensor);
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
