#include <zephyr/logging/log.h>

#include "utilities/memory/heap_allocator.h"
#include "subsys/fs/services/fs_service_stream_buf.h"
#include "subsys/device_tree/dt_canbus.h"

#include "canbus_service.h"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::device_tree;

LOG_MODULE_REGISTER(canbus_service_logger);

CanbusService::CanbusService(std::shared_ptr<IFsService> fs_service, std::shared_ptr<CanbusConfigurationManager> canbus_configuration_manager)
    : fs_service_(std::move(fs_service)), canbus_configuration_manager_(std::move(canbus_configuration_manager)) {

    for(const auto& [bus_channel, channel_configuration] : canbus_configuration_manager_->Get()->channel_configurations) {
        auto canbus = make_shared_ext<Canbus>(
            DtCanbus::Get(bus_channel),
            channel_configuration.type,
            channel_configuration.bitrate);

        if(!canbus->Initialize())
            throw std::runtime_error("Failed to initialize CANBus.");

        canbus->RegisterBitrateDetectedCallback([this, bus_channel](uint32_t bitrate) {
            BitrateUpdated(bus_channel, bitrate);
        });

        canbus_.insert({bus_channel, canbus});

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
}

void CanbusService::BitrateUpdated(uint8_t bus_channel, uint32_t bitrate) {
    auto canbus_configuration = canbus_configuration_manager_->Get();
    bool is_bus_channel_valid = canbus_configuration->channel_configurations.contains(bus_channel);

    if(is_bus_channel_valid && canbus_configuration_manager_->Update(*canbus_configuration))
        LOG_INF("Bitrate for bus channel %d updated to %d bps.", bus_channel, bitrate);
    else
        LOG_ERR("Failed to update bitrate for bus channel %d.", bus_channel);
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

} // namespace eerie_leap::domain::canbus_domain::services
