#pragma once

#include <vector>
#include <cstdint>

#include "domain/system_domain/models/system_configuration.h"

namespace eerie_leap::domain::system_domain::utilities::parsers {

using namespace eerie_leap::domain::system_domain::models;

class SystemConfigurationValidator {
public:
    static void ValidateCanbusConfigurations(const SystemConfiguration& configuration);
    static void ValidateSystemConfiguration(const SystemConfiguration& configuration);
};

} // namespace eerie_leap::domain::system_domain::utilities::parsers
