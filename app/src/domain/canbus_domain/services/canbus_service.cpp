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

    for(const auto& channel_configuration : canbus_configuration_manager_->Get()->channel_configurations) {
        auto canbus = make_shared_ext<Canbus>(
            DtCanbus::Get(channel_configuration.bus_channel),
            channel_configuration.type,
            channel_configuration.bitrate);

        if(!canbus->Initialize())
            throw std::runtime_error("Failed to initialize CANBus.");

        canbus->RegisterBitrateDetectedCallback([this, bus_channel = channel_configuration.bus_channel](uint32_t bitrate) {
            BitrateUpdated(bus_channel, bitrate);
        });

        canbus_.insert({channel_configuration.bus_channel, canbus});
        dbcs_.insert({channel_configuration.bus_channel, make_shared_ext<Dbc>()});

        std::string dbc_file_path = CONFIG_EERIE_LEAP_SD_CARD_USER_CONFIG_DIR;
        dbc_file_path += "/";
        dbc_file_path += CONFIG_EERIE_LEAP_CANBUS_DBC_FILE_NAME;
        dbc_file_path += std::to_string(channel_configuration.bus_channel);
        dbc_file_path += ".dbc";

        if(fs_service_ != nullptr && fs_service_->Exists(dbc_file_path)) {
            FsServiceStreamBuf fs_stream_buf(
                fs_service_.get(),
                dbc_file_path,
                FsServiceStreamBuf::OpenMode::Read);

            LoadDbcFile(channel_configuration.bus_channel, fs_stream_buf);

            fs_stream_buf.close();

            LOG_INF("DBC file loaded successfully. %s", dbc_file_path.c_str());
        } else {
            LOG_ERR("DBC file not found. %s", dbc_file_path.c_str());
        }
    }
}

void CanbusService::BitrateUpdated(uint8_t bus_channel, uint32_t bitrate) {
    bool is_bus_found = false;
    auto canbus_configuration = canbus_configuration_manager_->Get();

    for(auto& channel_configuration : canbus_configuration->channel_configurations) {
        if(channel_configuration.bus_channel == bus_channel) {
            channel_configuration.bitrate = bitrate;
            is_bus_found = true;
            break;
        }
    }

    if(is_bus_found && canbus_configuration_manager_->Update(*canbus_configuration))
        LOG_INF("Bitrate for bus channel %d updated to %d bps.", bus_channel, bitrate);
    else
        LOG_ERR("Failed to update bitrate for bus channel %d.", bus_channel);
}

bool CanbusService::LoadDbcFile(uint8_t bus_channel, std::streambuf& dbc_content) {
    bool res = dbcs_[bus_channel]->LoadDbcFile(dbc_content);
    if(res)
        LOG_INF("DBC file loaded successfully.");
    else
        LOG_ERR("Failed to load DBC file.");

    return res;
}

std::shared_ptr<Canbus> CanbusService::GetCanbus(uint8_t bus_channel) const {
    if(!canbus_.contains(bus_channel))
        return nullptr;

    return canbus_.at(bus_channel);
}

std::shared_ptr<Dbc> CanbusService::GetDbcForChannel(uint8_t bus_channel) const {
    if(!dbcs_.contains(bus_channel))
        return nullptr;

    return dbcs_.at(bus_channel);
}

} // namespace eerie_leap::domain::canbus_domain::services
