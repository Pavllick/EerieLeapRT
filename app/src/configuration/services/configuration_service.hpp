#pragma once

#include <memory>
#include <cstdint>
#include <string>
#include <optional>
#include <span>

#include "utilities/memory/heap_allocator.h"
#include "configuration/cbor_traits/system_config_trait.h"
#include "configuration/cbor_traits/adc_config_trait.h"
#include "configuration/cbor_traits/sensors_config_trait.h"
#include "domain/fs_domain/services/i_fs_service.h"
#include "utilities/cbor/cbor_serializer.hpp"
#include "loaded_config.hpp"
#include "utilities/logging/logger.hpp"

namespace eerie_leap::configuration::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::traits;
using namespace eerie_leap::domain::fs_domain::services;
using namespace eerie_leap::utilities::cbor;
using eerie_leap::utilities::logging::Logger;

template <typename T>
class ConfigurationService {
private:
    const std::string configuration_dir_ = "config";
    static constexpr size_t load_buffer_size_ = sizeof(T) + 2048;

    std::string configuration_name_;
    std::shared_ptr<IFsService> fs_service_;
    std::shared_ptr<CborSerializer<T>> cbor_serializer_;
    Logger logger_;

    const std::string configuration_file_path_ = configuration_dir_ + "/" + configuration_name_ + ".cbor";

public:
    ConfigurationService(std::string configuration_name, std::shared_ptr<IFsService> fs_service)
        : configuration_name_(std::move(configuration_name)),
          fs_service_(std::move(fs_service)),
          logger_(Logger(configuration_name_)) {

        cbor_serializer_ = make_shared_ext<CborSerializer<T>>(CborTrait<T>::Encode, CborTrait<T>::Decode);

        if (!fs_service_->Exists(configuration_dir_)) {
            logger_.Info("Configuration directory '%s' does not exist, creating it.", configuration_dir_.c_str());
            if (!fs_service_->CreateDirectory(configuration_dir_)) {
                logger_.Error("Failed to create configuration directory '%s'.", configuration_dir_.c_str());
            }
        }
    }

    bool Save(T* configuration) {
        auto config_bytes = cbor_serializer_->Serialize(*configuration);

        if (!config_bytes) {
            logger_.Error("Failed to serialize configuration!");
            return false;
        }

        logger_.Debug("Saving configuration to '%s', size: %zu bytes.", configuration_file_path_.c_str(), config_bytes->size());
        if (!fs_service_->WriteFile(configuration_file_path_, config_bytes->data(), config_bytes->size())) {
            logger_.Error("Failed to write file '%s'.", configuration_file_path_.c_str());
            return false;
        }
        logger_.Info("Successfully saved configuration to '%s'.", configuration_file_path_.c_str());
        return true;
    }

    std::optional<LoadedConfig<T>> Load() {
        if (!fs_service_->Exists(configuration_file_path_)) {
            logger_.Warning("Configuration file '%s' does not exist.", configuration_file_path_.c_str());
            return std::nullopt;
        }

        auto buffer = make_shared_ext<ExtVector>(load_buffer_size_);
        size_t out_len = 0;

        logger_.Debug("Reading configuration file '%s'.", configuration_file_path_.c_str());
        if (!fs_service_->ReadFile(configuration_file_path_, buffer->data(), load_buffer_size_, out_len)) {
            logger_.Error("Failed to read configuration file '%s'.", configuration_file_path_.c_str());
            return std::nullopt;
        }

        auto config_bytes = std::span<const uint8_t>(buffer->data(), out_len);
        auto configuration = cbor_serializer_->Deserialize(config_bytes);

        buffer->resize(out_len);

        if (!configuration.has_value()) {
            logger_.Error("Failed to deserialize configuration from '%s'.", configuration_file_path_.c_str());
            return std::nullopt;
        }

        logger_.Info("Successfully loaded configuration from '%s', size: %zu bytes.", configuration_file_path_.c_str(), out_len);

        auto config = make_shared_ext<T>(configuration.value());

        LoadedConfig<T> loaded_config {
            .config_raw = buffer,
            .config = config
        };

        return loaded_config;
    }
};

} // namespace eerie_leap::configuration::services
