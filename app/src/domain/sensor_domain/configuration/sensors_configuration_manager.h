#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

#include "utilities/memory/memory_resource_manager.h"
#include "subsys/fs/services/i_fs_service.h"
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "configuration/json/configs/json_sensors_config.h"
#include "configuration/services/json_configuration_service.h"
#include "domain/sensor_domain/configuration/parsers/sensors_cbor_parser.h"
#include "domain/sensor_domain/configuration/parsers/sensors_json_parser.h"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::configuration::parsers;

class SensorsConfigurationManager {
private:
    std::shared_ptr<IFsService> sd_fs_service_;

    std::unique_ptr<CborConfigurationService<CborSensorsConfig>> cbor_configuration_service_;
    std::unique_ptr<JsonConfigurationService<JsonSensorsConfig>> json_configuration_service_;

    std::unique_ptr<SensorsCborParser> cbor_parser_;
    std::unique_ptr<SensorsJsonParser> json_parser_;

    std::vector<std::shared_ptr<Sensor>> sensors_;
    int gpio_channel_count_;
    int adc_channel_count_;

    uint32_t json_config_checksum_;

    bool ApplyJsonConfiguration();
    bool CreateDefaultConfiguration();

public:
    SensorsConfigurationManager(
        std::shared_ptr<IFsService> sd_fs_service,
        std::unique_ptr<CborConfigurationService<CborSensorsConfig>> cbor_configuration_service,
        std::unique_ptr<JsonConfigurationService<JsonSensorsConfig>> json_configuration_service,
        int gpio_channel_count,
        int adc_channel_count);

    bool Update(const std::vector<std::shared_ptr<Sensor>>& sensors, bool internal_only = false);
    const std::vector<std::shared_ptr<Sensor>>* Get(bool force_load = false);
};

} // namespace eerie_leap::domain::sensor_domain::configuration
