#pragma once

#include <vector>
#include <memory>
#include <span>

#include "utilities/memory/heap_allocator.h"
#include "subsys/fs/services/i_fs_service.h"
#include "configuration/cbor/cbor_sensors_config/cbor_sensors_config.h"
#include "domain/sensor_domain/models/sensor.h"

namespace eerie_leap::domain::sensor_domain::configuration::utilities::parsers {

using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::sensor_domain::models;

class SensorsCborParser {
private:
    std::shared_ptr<IFsService> fs_service_;

public:
    explicit SensorsCborParser(std::shared_ptr<IFsService> fs_service);

    ext_unique_ptr<CborSensorsConfig> Serialize(
        const std::vector<std::shared_ptr<Sensor>>& sensors,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
    std::vector<std::shared_ptr<Sensor>> Deserialize(
        const CborSensorsConfig& sensors_config,
        uint32_t gpio_channel_count,
        uint32_t adc_channel_count);
};

} // namespace eerie_leap::domain::sensor_domain::configuration::utilities::parsers
