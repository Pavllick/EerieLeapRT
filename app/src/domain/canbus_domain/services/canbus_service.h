#pragma once

#include <memory>
#include <unordered_map>

#include "subsys/fs/services/i_fs_service.h"
#include "subsys/canbus/canbus.h"
#include "subsys/dbc/dbc.h"
#include "domain/canbus_domain/configuration/canbus_configuration_manager.h"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::subsys::dbc;
using namespace eerie_leap::domain::canbus_domain::configuration;

class CanbusService {
private:
    std::shared_ptr<IFsService> fs_service_;
    std::shared_ptr<CanbusConfigurationManager> canbus_configuration_manager_;

    std::unordered_map<uint8_t, std::shared_ptr<Canbus>> canbus_;
    std::unordered_map<uint8_t, std::shared_ptr<Dbc>> dbcs_;

    void BitrateUpdated(uint8_t bus_channel, uint32_t bitrate);

public:
    CanbusService(std::shared_ptr<IFsService> fs_service, std::shared_ptr<CanbusConfigurationManager> canbus_configuration_manager);

    bool LoadDbcFile(uint8_t bus_channel, std::streambuf& dbc_content);
    std::shared_ptr<Canbus> GetCanbus(uint8_t bus_channel) const;
    std::shared_ptr<Dbc> GetDbcForChannel(uint8_t bus_channel) const;
};

} // namespace eerie_leap::domain::canbus_domain::services
