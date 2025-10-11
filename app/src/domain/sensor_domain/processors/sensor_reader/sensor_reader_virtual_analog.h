#pragma once

#include <functional>
#include <memory>

#include "domain/sensor_domain/configuration/adc_configuration_manager.h"
#include "subsys/adc/i_adc_manager.h"
#include "subsys/adc/models/adc_configuration.h"
#include "domain/sensor_domain/models/sensor.h"
#include "sensor_reader_base.h"

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::domain::sensor_domain::configuration;

class SensorReaderVirtualAnalog : public SensorReaderBase {
public:
    SensorReaderVirtualAnalog(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> readings_frame,
        std::shared_ptr<Sensor> sensor);

    ~SensorReaderVirtualAnalog() override = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
