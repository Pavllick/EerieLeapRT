#pragma once

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "configuration/json/configs/json_canbus_config.h"
#include "subsys/fs/services/i_fs_service.h"
#include "domain/canbus_domain/models/canbus_configuration.h"

namespace eerie_leap::domain::canbus_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::canbus_domain::models;

class CanbusConfigurationJsonParser {
private:
    std::shared_ptr<IFsService> sd_fs_service_;

public:
    explicit CanbusConfigurationJsonParser(std::shared_ptr<IFsService> sd_fs_service);

    pmr_unique_ptr<JsonCanbusConfig> Serialize(const CanbusConfiguration& configuration);
    pmr_unique_ptr<CanbusConfiguration> Deserialize(std::pmr::memory_resource* mr, const JsonCanbusConfig& config);
};

} // namespace eerie_leap::domain::canbus_domain::configuration::parsers
