#pragma once

#include "subsys/fs/services/i_fs_service.h"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::configuration::parsers {

using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::sensor_domain::models;

class SensorValidator {
private:
    static void ValidateId(const std::vector<std::shared_ptr<Sensor>>& sensors);

    static void ValidateMetadata(const std::vector<std::shared_ptr<Sensor>>& sensors);
    static void ValidateName(const std::pmr::string& name);
    static void ValidateUnit(const std::pmr::string& unit);
    static void ValidateDescription(const std::pmr::string& description);

    static void ValidateSensorConfiguration(
        const std::vector<std::shared_ptr<Sensor>>& sensors,
        IFsService* sd_fs_service,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
    static void ValidateType(std::string_view sensor_id, const SensorConfiguration& sensor_configuration);
    static void ValidateChannel(
        std::string_view sensor_id,
        const SensorConfiguration& sensor_configuration,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
    static void ValidateConnectionString(std::string_view sensor_id, const SensorConfiguration& sensor_configuration);
    static void ValidateScriptPath(std::string_view sensor_id, const SensorConfiguration& sensor_configuration, IFsService* sd_fs_service);
    static void ValidateSamplingRateMs(std::string_view sensor_id, const SensorConfiguration& sensor_configuration);
    static void ValidateInterpolationMethod(std::string_view sensor_id, const SensorConfiguration& sensor_configuration);
    static void ValidateExpression(std::string_view sensor_id, const SensorConfiguration& sensor_configuration);

public:
    static void Validate(const std::vector<std::shared_ptr<Sensor>>& sensors, IFsService* sd_fs_service, uint32_t gpio_channel_count, uint32_t adc_channel_count);
};

} // namespace eerie_leap::domain::sensor_domain::configuration::parsers
