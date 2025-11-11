#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/system_config/system_config.h"
#include "domain/system_domain/models/system_configuration.h"

namespace eerie_leap::domain::system_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::system_domain::models;

class SystemConfigurationCborParser {
public:
    SystemConfigurationCborParser() = default;

    ext_unique_ptr<SystemConfig> Serialize(const SystemConfiguration& system_configuration);
    SystemConfiguration Deserialize(const SystemConfig& system_config);
};

} // namespace eerie_leap::domain::system_domain::utilities::parsers
