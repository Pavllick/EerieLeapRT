#pragma once

#include <memory>

#include "utilities/memory/memory_resource_manager.h"
#include "utilities/guid/guid_generator.h"
#include "subsys/time/i_time_service.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "i_sensor_reader.h"

namespace eerie_leap::domain::sensor_domain::sensor_readers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::subsys::time;
using namespace eerie_leap::domain::sensor_domain::utilities;

class SensorReaderBase : public ISensorReader {
protected:
    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<Sensor> sensor_;

public:
    SensorReaderBase(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::shared_ptr<Sensor> sensor)
            : time_service_(std::move(time_service)),
            guid_generator_(std::move(guid_generator)),
            sensor_readings_frame_(std::move(sensor_readings_frame)),
            sensor_(std::move(sensor)) {}

    virtual ~SensorReaderBase() = default;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
