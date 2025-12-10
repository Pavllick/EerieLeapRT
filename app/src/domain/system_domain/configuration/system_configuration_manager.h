#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor/cbor_system_config/cbor_system_config.h"
#include "configuration/json/configs/json_system_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "configuration/services/json_configuration_service.h"
#include "domain/system_domain/configuration/parsers/system_configuration_cbor_parser.h"
#include "domain/system_domain/configuration/parsers/system_configuration_json_parser.h"
#include "domain/system_domain/models/system_configuration.h"

namespace eerie_leap::domain::system_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::system_domain::configuration::parsers;
using namespace eerie_leap::domain::system_domain::models;

class SystemConfigurationManager {
private:
    std::unique_ptr<CborConfigurationService<CborSystemConfig>> cbor_configuration_service_;
    std::unique_ptr<JsonConfigurationService<JsonSystemConfig>> json_configuration_service_;

    ext_unique_ptr<SystemConfigurationCborParser> cbor_parser_;
    ext_unique_ptr<SystemConfigurationJsonParser> json_parser_;

    std::shared_ptr<SystemConfiguration> configuration_;

    uint32_t json_config_checksum_;

    bool UpdateHwVersion(uint32_t hw_version);
    bool UpdateSwVersion(uint32_t sw_version);
    bool ApplyJsonConfiguration();
    bool CreateDefaultConfiguration();

public:
    explicit SystemConfigurationManager(
        std::unique_ptr<CborConfigurationService<CborSystemConfig>> cbor_configuration_service,
        std::unique_ptr<JsonConfigurationService<JsonSystemConfig>> json_configuration_service);

    bool UpdateBuildNumber(uint32_t build_number);
    bool UpdateComUsers(const std::pmr::vector<ComUserConfiguration>& com_user_configurations);

    bool Update(const SystemConfiguration& configuration, bool internal_only = false);
    std::shared_ptr<SystemConfiguration> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::system_domain::configuration
