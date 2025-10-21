#pragma once

#include <memory>
#include <span>

#include "subsys/fs/services/i_fs_service.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/logging_domain/loggers/i_logger.h"

namespace eerie_leap::domain::logging_domain::loggers::simple_logger_sensor_reading {

using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::sensor_domain::models;

class SimpleLoggerSensorReading : public ILogger<SensorReading> {
private:
    std::shared_ptr<IFsService> fs_service_;
    std::streambuf* stream_;
    uint32_t log_metadata_file_version_;
    system_clock::time_point start_time_;

public:
    SimpleLoggerSensorReading(std::shared_ptr<IFsService> fs_service);
    virtual ~SimpleLoggerSensorReading() = default;

    bool CreateMetadataFile(const std::span<uint8_t>& sensors_metadata);

    const char* GetFileExtension() const override;
    bool StartLogging(std::streambuf& stream, const system_clock::time_point& start_time) override;
    bool StopLogging() override;
    bool LogReading(const system_clock::time_point& time, const SensorReading& reading) override;
};

} // namespace eerie_leap::domain::logging_domain::loggers::simple_logger_sensor_reading
