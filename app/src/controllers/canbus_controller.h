#pragma once

#include <memory>
#include <fstream>
#include <unordered_map>

#include "subsys/fs/services/fs_service.h"
#include "subsys/canbus/canbus.h"
#include "subsys/dbc/dbc.h"
#include "domain/system_domain/configuration/system_configuration_manager.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::subsys::dbc;
using namespace eerie_leap::domain::system_domain::configuration;

class CanbusController {
private:
    std::shared_ptr<IFsService> fs_service_;
    std::shared_ptr<SystemConfigurationManager> system_configuration_manager_;

    std::unordered_map<uint8_t, std::shared_ptr<Canbus>> canbus_;
    std::shared_ptr<Dbc> dbc_;

public:
    CanbusController(std::shared_ptr<IFsService> fs_service, std::shared_ptr<SystemConfigurationManager> system_configuration_manager);

    bool LoadDbcFile(std::streambuf& dbc_content);
    std::shared_ptr<Canbus> GetCanbus(uint8_t bus_channel) const;
    std::shared_ptr<Dbc> GetDbc() const;
};

} // namespace eerie_leap::controllers
