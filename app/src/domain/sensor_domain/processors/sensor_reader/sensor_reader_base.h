#pragma once

#include <memory>

#include "subsys/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "i_sensor_reader.h"

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

using namespace eerie_leap::subsys::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorReaderBase : public ISensorReader {
protected:
    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<SensorReadingsFrame> readings_frame_;
    std::shared_ptr<Sensor> sensor_;

public:
    SensorReaderBase(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> readings_frame,
        std::shared_ptr<Sensor> sensor)
        : time_service_(std::move(time_service)),
        guid_generator_(std::move(guid_generator)),
        readings_frame_(std::move(readings_frame)),
        sensor_(std::move(sensor)) {}

    virtual ~SensorReaderBase() = default;
};

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
