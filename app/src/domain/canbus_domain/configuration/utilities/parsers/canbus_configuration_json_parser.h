#pragma once

#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"
#include "configuration/json/configs/json_canbus_config.h"
#include "subsys/fs/services/i_fs_service.h"
#include "domain/canbus_domain/models/canbus_configuration.h"

namespace eerie_leap::domain::canbus_domain::configuration::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::canbus_domain::models;

class CanbusConfigurationJsonParser {
private:
    std::shared_ptr<IFsService> fs_service_;

public:
    explicit CanbusConfigurationJsonParser(std::shared_ptr<IFsService> fs_service);

    ext_unique_ptr<JsonCanbusConfig> Serialize(const CanbusConfiguration& configuration);
    CanbusConfiguration Deserialize(const JsonCanbusConfig& config);
};

} // namespace eerie_leap::domain::canbus_domain::configuration::utilities::parsers
