#pragma once

#include <memory>
#include <unordered_map>

#include "subsys/mdf/mdf4_file.h"
#include "subsys/mdf/mdf4/data_record.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "i_logger.h"

namespace eerie_leap::domain::logging_domain::loggers {

using namespace eerie_leap::subsys::mdf;
using namespace eerie_leap::subsys::mdf::mdf4;
using namespace eerie_leap::domain::sensor_domain::models;

class Mdf4LoggerSensorReading : public ILogger<SensorReading> {
private:
    static constexpr uint8_t RECORD_ID_SIZE = 4;

    std::unique_ptr<Mdf4File> mdf4_file_;
    std::unordered_map<uint32_t, std::unique_ptr<DataRecord>> records_;
    std::streambuf* stream_;
    system_clock::time_point start_time_;

public:
    explicit Mdf4LoggerSensorReading(const std::vector<std::shared_ptr<Sensor>>& sensors);
    virtual ~Mdf4LoggerSensorReading() = default;

    const char* GetFileExtension() const override;
    bool StartLogging(std::streambuf& stream, const system_clock::time_point& start_time) override;
    bool StopLogging() override;
    bool LogReading(const system_clock::time_point& time, const SensorReading& reading) override;
};

} // namespace eerie_leap::domain::logging_domain::loggers
