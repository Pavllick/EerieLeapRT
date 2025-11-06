#pragma once

#include <memory>
#include <unordered_map>
#include <chrono>

#include "subsys/mdf/mdf4_file.h"
#include "subsys/mdf/mdf4/channel_group_block.h"
#include "subsys/mdf/mdf4/data_record.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "i_logger.h"

namespace eerie_leap::domain::logging_domain::loggers {

using namespace std::chrono;
using namespace eerie_leap::subsys::mdf;
using namespace eerie_leap::subsys::mdf::mdf4;
using namespace eerie_leap::domain::sensor_domain::models;

class Mdf4LoggerSensorReading : public ILogger<SensorReading> {
private:
    static constexpr uint8_t RECORD_ID_SIZE = 4;

    std::unique_ptr<Mdf4File> mdf4_file_;
    std::unordered_map<uint32_t, std::unique_ptr<DataRecord>> records_;
    std::unordered_map<uint32_t, std::shared_ptr<ChannelGroupBlock>> can_raw_channel_groups_;
    std::streambuf* stream_;
    system_clock::time_point start_time_;

    uint32_t vlsd_channel_group_id_ = 0;
    uint64_t current_file_size_bytes_;
    system_clock::time_point last_can_reading_time_;

    bool LogValueReading(float time_delta_s, const SensorReading& reading);
    bool LogCanbusRawReading(float time_delta_s, const SensorReading& reading);

public:
    explicit Mdf4LoggerSensorReading(const std::vector<std::shared_ptr<Sensor>>& sensors);
    virtual ~Mdf4LoggerSensorReading() = default;

    const char* GetFileExtension() const override;
    bool StartLogging(std::streambuf& stream, const system_clock::time_point& start_time) override;
    bool StopLogging() override;
    bool LogReading(const system_clock::time_point& time, const SensorReading& reading) override;
};

} // namespace eerie_leap::domain::logging_domain::loggers
