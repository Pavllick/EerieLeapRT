#pragma once

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_canbus_config/cbor_canbus_config.h"
#include "domain/canbus_domain/models/canbus_configuration.h"

namespace eerie_leap::domain::canbus_domain::utilities::parsers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::canbus_domain::models;

class CanbusConfigurationCborParser {
public:
    CanbusConfigurationCborParser() = default;

    ext_unique_ptr<CborCanbusConfig> Serialize(const CanbusConfiguration& configuration);
    CanbusConfiguration Deserialize(const CborCanbusConfig& config);
};

} // namespace eerie_leap::domain::canbus_domain::utilities::parsers
