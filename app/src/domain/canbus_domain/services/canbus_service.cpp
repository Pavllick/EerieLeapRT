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

        LoadDbcConfiguration(channel_configuration);
        ConfigureUserSignals(channel_configuration);
    }
}

bool CanbusService::LoadDbcFile(Dbc& dbc, std::streambuf& dbc_content) {
    return dbc.LoadDbcFile(dbc_content);
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

void CanbusService::LoadDbcConfiguration(const CanChannelConfiguration& channel_configuration) {
    if(fs_service_ != nullptr && fs_service_->Exists(channel_configuration.dbc_file_path)) {
        FsServiceStreamBuf fs_stream_buf(
            fs_service_.get(),
            channel_configuration.dbc_file_path,
            FsServiceStreamBuf::OpenMode::Read);

        bool res = LoadDbcFile(*channel_configuration.dbc, fs_stream_buf);

        fs_stream_buf.close();

        if(res)
            LOG_INF("DBC file loaded successfully. %s", channel_configuration.dbc_file_path.c_str());
        else
            LOG_ERR("Failed to load DBC file. %s", channel_configuration.dbc_file_path.c_str());
    } else if(fs_service_ != nullptr && !channel_configuration.dbc_file_path.empty()) {
        LOG_ERR("DBC file not found. %s", channel_configuration.dbc_file_path.c_str());
    }
}

void CanbusService::ConfigureUserSignals(const CanChannelConfiguration& channel_configuration) {
    for(const auto& message_configuration : channel_configuration.message_configurations) {
        DbcMessage* message = nullptr;

        if(channel_configuration.dbc->HasMessage(message_configuration.frame_id))
            message = channel_configuration.dbc->GetOrRegisterMessage(message_configuration.frame_id);
        else
            message = channel_configuration.dbc->AddMessage(
                message_configuration.frame_id,
                message_configuration.name,
                message_configuration.message_size);

        for(const auto& signal_configuration : message_configuration.signal_configurations) {
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
