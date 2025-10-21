#include <zephyr/sys/crc.h>
#include <zephyr/logging/log.h>

#include "utilities/constants.h"
#include "domain/logging_domain/loggers/simple_logger_sensor_reading/models/log_metadata_header.h"
#include "domain/logging_domain/loggers/simple_logger_sensor_reading/models/log_data_header.h"
#include "domain/logging_domain/loggers/simple_logger_sensor_reading/models/log_data_record.h"

#include "simple_logger_sensor_reading.h"

namespace eerie_leap::domain::logging_domain::loggers::simple_logger_sensor_reading {

using namespace eerie_leap::utilities::constants::logging;
using namespace eerie_leap::domain::logging_domain::loggers::simple_logger_sensor_reading::models;

LOG_MODULE_REGISTER(simple_logger_sensor_reading);

SimpleLoggerSensorReading::SimpleLoggerSensorReading(std::shared_ptr<IFsService> fs_service)
    : fs_service_(fs_service), stream_(nullptr) { }

bool SimpleLoggerSensorReading::CreateMetadataFile(const std::span<uint8_t>& sensors_metadata) {
    auto log_metadata_header = LogMetadataHeader::Create();

    if(!fs_service_->Exists(CONFIG_EERIE_LEAP_LOG_METADATA_FILE_DIR))
        if(!fs_service_->CreateDirectory(CONFIG_EERIE_LEAP_LOG_METADATA_FILE_DIR)) {
            LOG_ERR("Failed to create %s directory", CONFIG_EERIE_LEAP_LOG_METADATA_FILE_DIR);
            return -1;
        }

    uint32_t crc = crc32_ieee((uint8_t*)(&log_metadata_header), sizeof(log_metadata_header));
    crc = crc32_ieee_update(crc, sensors_metadata.data(), sensors_metadata.size());
    log_metadata_file_version_ = crc;

    std::string version_str;
    version_str.resize(sizeof(log_metadata_file_version_) * 2);
    snprintf(version_str.data(), version_str.size() + 1, "%08x", log_metadata_file_version_);

    std::string path = CONFIG_EERIE_LEAP_LOG_METADATA_FILE_DIR;
    path += "/";
    path += CONFIG_EERIE_LEAP_LOG_METADATA_FILE_NAME;
    path += "_";
    path += version_str;
    path += ".";
    path += LOG_METADATA_FILE_EXTENSION;

    if(fs_service_->Exists(path))
        return 0;

    bool success = fs_service_->WriteFile(path, &log_metadata_header, sizeof(log_metadata_header));
    if(!success) {
        LOG_ERR("Failed to save log metadata header.");
        return -1;
    }

    success = fs_service_->WriteFile(path, sensors_metadata.data(), sensors_metadata.size(), true);
    if(!success) {
        LOG_ERR("Failed to save log metadata.");
        return -1;
    }

    success = fs_service_->WriteFile(path, &crc, sizeof(crc), true);
    if(!success) {
        LOG_ERR("Failed to save log metadata CRC.");
        return -1;
    }

    LOG_INF("Log metadata saved successfully, %s", path.c_str());

    return 0;
}

const char* SimpleLoggerSensorReading::GetFileExtension() const {
    return LOG_DATA_FILE_EXTENSION;
}

bool SimpleLoggerSensorReading::StartLogging(std::streambuf& stream, const system_clock::time_point& start_time) {
    stream_ = &stream;
    start_time_ = start_time;

    auto log_data_header = LogDataHeader::Create(start_time, log_metadata_file_version_);

    auto ret = stream.sputn(
        reinterpret_cast<const char*>(&log_data_header),
        static_cast<std::streamsize>(sizeof(log_data_header)));

    if(ret != sizeof(log_data_header)) {
        LOG_ERR("Failed to save log data header.");
        return false;
    }

    return true;
}

bool SimpleLoggerSensorReading::StopLogging() {
    stream_ = nullptr;

    return true;
}

bool SimpleLoggerSensorReading::LogReading(const system_clock::time_point& time, const SensorReading& reading) {
    if(!stream_)
        return false;

    if(!reading.value.has_value())
        return false;

    uint32_t value = 0;
    float float_value = reading.value.value();
    memcpy(&value, &float_value, sizeof(value));

    auto log_record = LogDataRecord<uint32_t>::Create(
        time - start_time_,
        reading.sensor->id_hash,
        value);

    auto ret = stream_->sputn(
        reinterpret_cast<const char*>(&log_record),
        static_cast<std::streamsize>(sizeof(log_record)));

    if(ret != sizeof(log_record)) {
        LOG_ERR("Failed to save log record.");
        return false;
    }

    return true;
}

} // namespace eerie_leap::domain::logging_domain::loggers::simple_logger_sensor_reading
