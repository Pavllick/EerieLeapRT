#pragma once

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_system_config/cbor_system_config.h"
#include "domain/system_domain/models/system_configuration.h"

namespace eerie_leap::domain::system_domain::configuration::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::system_domain::models;

class SystemConfigurationCborParser {
public:
    SystemConfigurationCborParser() = default;

    ext_unique_ptr<CborSystemConfig> Serialize(const SystemConfiguration& configuration);
    SystemConfiguration Deserialize(const CborSystemConfig& config);
};

} // namespace eerie_leap::domain::system_domain::configuration::parsers
