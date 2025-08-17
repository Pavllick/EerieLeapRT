#pragma once

#include <functional>
#include <memory>

#include "controllers/adc_configuration_controller.h"
#include "subsys/adc/i_adc_manager.h"
#include "subsys/adc/models/adc_configuration.h"
#include "domain/sensor_domain/models/sensor.h"
#include "sensor_reader_base.h"

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

using namespace eerie_leap::controllers;
using namespace eerie_leap::subsys::adc;

class SensorReaderPhysicalAnalog : public SensorReaderBase {
private:
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller_;

    std::shared_ptr<IAdcManager> adc_manager_;
    std::shared_ptr<AdcChannelConfiguration> adc_channel_configuration_;

protected:
    std::function<float ()> AdcChannelReader;

public:
    SensorReaderPhysicalAnalog(
        std::shared_ptr<ITimeService>& time_service,
        std::shared_ptr<GuidGenerator>& guid_generator,
        std::shared_ptr<SensorReadingsFrame>& readings_frame,
        std::shared_ptr<Sensor>& sensor,
        std::shared_ptr<AdcConfigurationController>& adc_configuration_controller);

    ~SensorReaderPhysicalAnalog() override = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
