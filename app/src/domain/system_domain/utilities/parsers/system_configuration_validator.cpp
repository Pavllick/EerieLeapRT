#include <stdexcept>
#include <unordered_set>

#include "system_configuration_validator.h"

namespace eerie_leap::domain::system_domain::utilities::parsers {

void SystemConfigurationValidator::ValidateCanbusConfigurations(const SystemConfiguration& configuration) {
    std::unordered_set<uint32_t> can_channels;

    for(const auto& canbus_configuration : configuration.canbus_configurations) {
        if(can_channels.contains(canbus_configuration.bus_channel))
            throw std::runtime_error("Duplicate CAN bus channel");
        can_channels.insert(canbus_configuration.bus_channel);
    }
}

void SystemConfigurationValidator::ValidateSystemConfiguration(const SystemConfiguration& configuration) {
    ValidateCanbusConfigurations(configuration);
}

} // namespace eerie_leap::domain::system_domain::utilities::parsers
