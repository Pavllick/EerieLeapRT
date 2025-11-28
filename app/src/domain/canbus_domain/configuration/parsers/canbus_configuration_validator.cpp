#include <stdexcept>
#include <string>
#include <unordered_set>

#include "canbus_configuration_validator.h"
#include "subsys/canbus/canbus.h"

namespace eerie_leap::domain::canbus_domain::configuration::parsers {

using namespace eerie_leap::subsys::canbus;

void CanbusConfigurationValidator::Validate(const CanbusConfiguration& configuration) {
    std::unordered_set<uint8_t> can_channels;

    for(const auto& [_, canbus_configuration] : configuration.channel_configurations) {
        if(can_channels.contains(canbus_configuration.bus_channel))
            throw std::runtime_error("Duplicate CAN bus channel " + std::to_string(canbus_configuration.bus_channel));
        can_channels.insert(canbus_configuration.bus_channel);
    }

    for(const auto& [_, canbus_configuration] : configuration.channel_configurations) {
        if(!Canbus::IsBitrateSupported(canbus_configuration.type, canbus_configuration.bitrate))
            throw std::runtime_error("Invalid CAN bus bitrate. Channel: " + std::to_string(canbus_configuration.bus_channel));
    }
}

} // namespace eerie_leap::domain::canbus_domain::configuration::parsers
