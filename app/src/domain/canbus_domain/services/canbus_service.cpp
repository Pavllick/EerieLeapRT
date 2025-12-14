#include <zephyr/logging/log.h>

#include "subsys/fs/services/fs_service_stream_buf.h"
#include "subsys/device_tree/dt_canbus.h"

#include "canbus_service.h"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::subsys::device_tree;

LOG_MODULE_REGISTER(canbus_service_logger);

CanbusService::CanbusService(std::shared_ptr<IFsService> fs_service, std::shared_ptr<CanbusConfigurationManager> canbus_configuration_manager)
    : fs_service_(std::move(fs_service)), canbus_configuration_manager_(std::move(canbus_configuration_manager)) {

    for(const auto& [bus_channel, channel_configuration] : canbus_configuration_manager_->Get()->channel_configurations) {
        auto canbus = std::make_shared<Canbus>(
            DtCanbus::Get(bus_channel),
            channel_configuration.type,
            channel_configuration.bitrate,
            channel_configuration.data_bitrate,
            channel_configuration.is_extended_id);

        if(!canbus->Initialize()) {
            LOG_ERR("Failed to initialize CAN channel %d.", bus_channel);
            continue;
        }

        canbus->RegisterBitrateDetectedCallback([this, bus_channel](uint32_t bitrate) {
            BitrateUpdated(bus_channel, bitrate);
        });

        canbus_.emplace(bus_channel, canbus);

        ConfigureUserSignals(channel_configuration);
    }
}

std::shared_ptr<Canbus> CanbusService::GetCanbus(uint8_t bus_channel) const {
    if(!canbus_.contains(bus_channel))
        return nullptr;

    return canbus_.at(bus_channel);
}

const CanChannelConfiguration* CanbusService::GetChannelConfiguration(uint8_t bus_channel) const {
    auto canbus_configuration = canbus_configuration_manager_->Get();

    if(!canbus_configuration->channel_configurations.contains(bus_channel))
        return nullptr;

    return &canbus_configuration->channel_configurations.at(bus_channel);
}

void CanbusService::BitrateUpdated(uint8_t bus_channel, uint32_t bitrate) {
    auto canbus_configuration = canbus_configuration_manager_->Get();
    bool is_bus_channel_valid = canbus_configuration->channel_configurations.contains(bus_channel);

    if(is_bus_channel_valid && canbus_configuration_manager_->Update(*canbus_configuration))
        LOG_INF("Bitrate for bus channel %d updated to %d bps.", bus_channel, bitrate);
    else
        LOG_ERR("Failed to update bitrate for bus channel %d.", bus_channel);
}

void CanbusService::ConfigureUserSignals(const CanChannelConfiguration& channel_configuration) {
    for(const auto& message_configuration : channel_configuration.message_configurations) {
        DbcMessage* message = nullptr;

        if(channel_configuration.dbc->HasMessage(message_configuration->frame_id))
            message = channel_configuration.dbc->GetMessage(message_configuration->frame_id);
        else
            message = channel_configuration.dbc->AddMessage(
                message_configuration->frame_id,
                message_configuration->name,
                message_configuration->message_size);

        for(const auto& signal_configuration : message_configuration->signal_configurations) {
            message->AddSignal(
                signal_configuration.name,
                signal_configuration.start_bit,
                signal_configuration.size_bits,
                signal_configuration.factor,
                signal_configuration.offset,
                signal_configuration.unit);
        }
    }
}

} // namespace eerie_leap::domain::canbus_domain::services
