#pragma once

#include <functional>
#include <memory>

#include "domain/sensor_domain/configuration/adc_configuration_manager.h"
#include "subsys/adc/i_adc_manager.h"
#include "subsys/adc/models/adc_configuration.h"
#include "domain/sensor_domain/models/sensor.h"
#include "sensor_reader_base.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::subsys::adc;
using namespace eerie_leap::domain::sensor_domain::configuration;

class SensorReaderPhysicalAnalog : public SensorReaderBase {
private:
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager_;

    std::shared_ptr<IAdcManager> adc_manager_;
    std::shared_ptr<AdcChannelConfiguration> adc_channel_configuration_;

protected:
    std::function<float ()> AdcChannelReader;

public:
    SensorReaderPhysicalAnalog(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<Sensor> sensor,
        std::shared_ptr<AdcConfigurationManager> adc_configuration_manager);

    ~SensorReaderPhysicalAnalog() override = default;

    void Read() override;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
