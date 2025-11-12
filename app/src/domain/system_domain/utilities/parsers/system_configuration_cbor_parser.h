#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_system_config/cbor_system_config.h"
#include "domain/system_domain/models/system_configuration.h"

namespace eerie_leap::domain::system_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::system_domain::models;

class SystemConfigurationCborParser {
public:
    SystemConfigurationCborParser() = default;

    ext_unique_ptr<CborSystemConfig> Serialize(const SystemConfiguration& system_configuration);
    SystemConfiguration Deserialize(const CborSystemConfig& system_config);
};

} // namespace eerie_leap::domain::system_domain::utilities::parsers
