#include <zephyr/logging/log.h>

#include "utilities/memory/heap_allocator.h"
#include "subsys/fs/services/fs_service_stream_buf.h"
#include "subsys/device_tree/dt_canbus.h"

#include "canbus_service.h"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::device_tree;

LOG_MODULE_REGISTER(canbus_service_logger);

CanbusService::CanbusService(std::shared_ptr<IFsService> fs_service, std::shared_ptr<SystemConfigurationManager> system_configuration_manager)
    : fs_service_(std::move(fs_service)), system_configuration_manager_(std::move(system_configuration_manager)) {

    for(const auto& canbus_configuration : system_configuration_manager_->Get()->canbus_configurations) {
        auto canbus = make_shared_ext<Canbus>(
                DtCanbus::Get(canbus_configuration.bus_channel),
                canbus_configuration.bitrate);

        if(!canbus->Initialize())
            throw std::runtime_error("Failed to initialize CANBus.");

        canbus->RegisterBitrateDetectedCallback([this, bus_channel = canbus_configuration.bus_channel](uint32_t bitrate) {
            BitrateUpdated(bus_channel, bitrate);
        });

        canbus_.insert({canbus_configuration.bus_channel, canbus});
    }

    dbc_ = make_shared_ext<Dbc>();

    if(fs_service_ != nullptr && fs_service_->Exists(CONFIG_EERIE_LEAP_CANBUS_DBC_FILE_PATH)) {
        FsServiceStreamBuf fs_stream_buf(
            fs_service_.get(),
            CONFIG_EERIE_LEAP_CANBUS_DBC_FILE_PATH,
            FsServiceStreamBuf::OpenMode::Read);
        LoadDbcFile(fs_stream_buf);

        fs_stream_buf.close();
    }
}

void CanbusService::BitrateUpdated(uint8_t bus_channel, uint32_t bitrate) {
    bool is_bus_found = false;
    auto system_configuration = system_configuration_manager_->Get();

    for(auto& can_bus : system_configuration->canbus_configurations) {
        if(can_bus.bus_channel == bus_channel) {
            can_bus.bitrate = bitrate;
            is_bus_found = true;
            break;
        }
    }

    if(is_bus_found && system_configuration_manager_->Update(system_configuration))
        LOG_INF("Bitrate for bus channel %d updated to %d bps.", bus_channel, bitrate);
    else
        LOG_ERR("Failed to update bitrate for bus channel %d.", bus_channel);
}

bool CanbusService::LoadDbcFile(std::streambuf& dbc_content) {
    bool res = dbc_->LoadDbcFile(dbc_content);
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

std::shared_ptr<Dbc> CanbusService::GetDbc() const {
    return dbc_;
}

} // namespace eerie_leap::domain::canbus_domain::services
