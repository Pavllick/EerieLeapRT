#pragma once

#include "utilities/memory/memory_resource_manager.h"
#include "configuration/cbor/cbor_canbus_config/cbor_canbus_config.h"
#include "subsys/fs/services/i_fs_service.h"
#include "domain/canbus_domain/models/canbus_configuration.h"

namespace eerie_leap::domain::canbus_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::canbus_domain::models;

class CanbusConfigurationCborParser {
private:
    std::shared_ptr<IFsService> sd_fs_service_;

public:
    explicit CanbusConfigurationCborParser(std::shared_ptr<IFsService> sd_fs_service);

    pmr_unique_ptr<CborCanbusConfig> Serialize(const CanbusConfiguration& configuration);
    pmr_unique_ptr<CanbusConfiguration> Deserialize(std::pmr::memory_resource* mr, const CborCanbusConfig& config);
};

} // namespace eerie_leap::domain::canbus_domain::configuration::parsers
