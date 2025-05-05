#pragma once

#include <memory>
#include <string>
#include <optional>
#include <span>
#include <zephyr/logging/log.h>

#include "configuration/cbor_traits/system_config_trait.h"
#include <configuration/cbor_traits/sensors_config_trait.h>
#include "domain/fs_domain/services/i_fs_service.h"
#include "utilities/cbor/cbor_serializer.hpp"

namespace eerie_leap::configuration::services {

// TODO: Figure out logging without LOG_MODULE_REGISTER and LOG_INSTANCE_PTR_DECLARE

using namespace eerie_leap::configuration::traits;
using namespace eerie_leap::domain::fs_domain::services;
using namespace eerie_leap::utilities::cbor;

template <typename T>
class ConfigurationService {
private:
    const std::string configuration_dir_ = "config";
    static const size_t max_config_buffer_size_ = 512;

    std::string configuration_name_;
    std::shared_ptr<IFsService> fs_service_;
    std::shared_ptr<CborSerializer<T, sizeof(T)>> cbor_serializer_;

    const std::string configuration_file_path_ = configuration_dir_ + "/" + configuration_name_ + ".cbor";
    
public:
    ConfigurationService(std::string configuration_name, std::shared_ptr<IFsService> fs_service) 
        : configuration_name_(configuration_name), fs_service_(std::move(fs_service)) {

        cbor_serializer_ = std::make_shared<CborSerializer<T, sizeof(T)>>(CborTrait<T>::Encode, CborTrait<T>::Decode);

        if (!fs_service_->Exists(configuration_dir_))
            fs_service_->CreateDirectory(configuration_dir_);
    }

    bool Save(T &configuration) {
        auto config_bytes = cbor_serializer_->Serialize(configuration);

        if (!config_bytes) {
            // LOG_ERR("Failed to serialize configuration!");
            return false;
        }

        return fs_service_->WriteFile(configuration_file_path_, config_bytes.value().data(), config_bytes.value().size());
    }

    std::optional<T> Load() {
        if (!fs_service_->Exists(configuration_file_path_)) {
            // LOG_ERR("Configuration file does not exist!");
            return std::nullopt;
        }

        std::array<uint8_t, max_config_buffer_size_> buffer = {};
        size_t out_len = 0;

        if (!fs_service_->ReadFile(configuration_file_path_, buffer.data(), buffer.size(), out_len)) {
            // LOG_ERR("Failed to read configuration file!");
            return std::nullopt;
        }

        auto config_bytes = std::span<const uint8_t>(buffer.data(), out_len);
        auto configuration = cbor_serializer_->Deserialize(config_bytes);

        if (!configuration.has_value()) {
            // LOG_ERR("Failed to deserialize configuration!");
            return std::nullopt;
        }

        return configuration;
    }
};

} // namespace eerie_leap::configuration::services