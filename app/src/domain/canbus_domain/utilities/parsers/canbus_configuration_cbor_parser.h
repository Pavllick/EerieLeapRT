#pragma once

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_canbus_config/cbor_canbus_config.h"
#include "subsys/fs/services/i_fs_service.h"
#include "domain/canbus_domain/models/canbus_configuration.h"

namespace eerie_leap::domain::canbus_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::canbus_domain::models;

class CanbusConfigurationCborParser {
private:
    std::shared_ptr<IFsService> fs_service_;

public:
    explicit CanbusConfigurationCborParser(std::shared_ptr<IFsService> fs_service);

    ext_unique_ptr<CborCanbusConfig> Serialize(const CanbusConfiguration& configuration);
    CanbusConfiguration Deserialize(const CborCanbusConfig& config);
};

} // namespace eerie_leap::domain::canbus_domain::utilities::parsers
