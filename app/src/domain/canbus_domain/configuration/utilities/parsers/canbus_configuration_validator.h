#pragma once

#include "domain/canbus_domain/models/canbus_configuration.h"

namespace eerie_leap::domain::canbus_domain::configuration::utilities::parsers {

using namespace eerie_leap::domain::canbus_domain::models;

class CanbusConfigurationValidator {
public:
    static void Validate(const CanbusConfiguration& configuration);
};

} // namespace eerie_leap::domain::canbus_domain::configuration::utilities::parsers
