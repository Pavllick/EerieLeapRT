#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "configuration/system_config/system_config.h"
#include "configuration/services/cbor_configuration_service.h"
#include "domain/system_domain/utilities/parsers/system_configuration_cbor_parser.h"
#include "domain/system_domain/utilities/parsers/system_configuration_json_parser.h"
#include "domain/system_domain/models/system_configuration.h"

namespace eerie_leap::domain::system_domain::configuration {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::services;
using namespace eerie_leap::domain::system_domain::utilities::parsers;
using namespace eerie_leap::domain::system_domain::models;

class SystemConfigurationManager {
private:
    ext_unique_ptr<CborConfigurationService<SystemConfig>> cbor_configuration_service_;

    ext_unique_ptr<ExtVector> config_raw_;
    ext_unique_ptr<SystemConfig> config_;
    std::shared_ptr<SystemConfiguration> configuration_;
    std::unique_ptr<SystemConfigurationCborParser> cbor_parser_;
    std::unique_ptr<SystemConfigurationJsonParser> json_parser_;

    uint32_t sd_json_checksum_;

    bool UpdateHwVersion(uint32_t hw_version);
    bool UpdateSwVersion(uint32_t sw_version);
    bool CreateDefaultConfiguration();

public:
    explicit SystemConfigurationManager(ext_unique_ptr<CborConfigurationService<SystemConfig>> cbor_configuration_service);

    bool UpdateBuildNumber(uint32_t build_number);
    bool UpdateComUsers(const std::vector<ComUserConfiguration>& com_user_configurations);

    bool Update(std::shared_ptr<SystemConfiguration> configuration);
    std::shared_ptr<SystemConfiguration> Get(bool force_load = false);
};

} // namespace eerie_leap::domain::system_domain::configuration
