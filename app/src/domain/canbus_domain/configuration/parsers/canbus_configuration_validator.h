#pragma once

#include "subsys/fs/services/i_fs_service.h"
#include "domain/canbus_domain/models/canbus_configuration.h"

namespace eerie_leap::domain::canbus_domain::configuration::parsers {

using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::domain::canbus_domain::models;

class CanbusConfigurationValidator {
private:
    static void ValidateChannelType(const CanbusConfiguration& configuration);
    static void ValidateIsExtendedId(const CanbusConfiguration& configuration);
    static void ValidateBitrate(const CanbusConfiguration& configuration);
    static void ValidateDataBitrate(const CanbusConfiguration& configuration);
    static void ValidateDbcFilePath(const CanbusConfiguration& configuration, IFsService* sd_fs_service);

    static void ValidateMessages(const CanbusConfiguration& configuration, IFsService* sd_fs_service);
    static void ValidateMessageFrameId(const CanChannelConfiguration& channel_configuration);
    static void ValidateMessageSendIntervalMs(const CanChannelConfiguration& channel_configuration);
    static void ValidateMessageScriptPath(const CanChannelConfiguration& channel_configuration, IFsService* sd_fs_service);
    static void ValidateMessageName(const CanChannelConfiguration& channel_configuration);
    static void ValidateMessageSize(const CanChannelConfiguration& channel_configuration, CanbusType type);

    static void ValidateSignals(const CanChannelConfiguration& channel_configuration);
    static void ValidateSignalStartBit(const CanMessageConfiguration& message_configuration, uint8_t bus_channel);
    static void ValidateSignalSizeBits(const CanMessageConfiguration& message_configuration, uint8_t bus_channel);
    static void ValidateSignalFactor(const CanMessageConfiguration& message_configuration, uint8_t bus_channel);
    static void ValidateSignalOffset(const CanMessageConfiguration& message_configuration, uint8_t bus_channel);
    static void ValidateSignalName(const CanMessageConfiguration& message_configuration, uint8_t bus_channel);
    static void ValidateSignalUnit(const CanMessageConfiguration& message_configuration, uint8_t bus_channel);

public:
    static void Validate(const CanbusConfiguration& configuration, IFsService* sd_fs_service);
};

} // namespace eerie_leap::domain::canbus_domain::configuration::parsers
