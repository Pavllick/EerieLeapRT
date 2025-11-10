#pragma once

#include <memory>
#include <cstdint>
#include <string>
#include <optional>
#include <span>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "utilities/memory/heap_allocator.h"
#include "utilities/cbor/cbor_serializer.h"
#include "subsys/fs/services/i_fs_service.h"

#include "configuration/cbor_traits/system_config_trait.h"
#include "configuration/cbor_traits/adc_config_trait.h"
#include "configuration/cbor_traits/sensors_config_trait.h"
#include "configuration/cbor_traits/logging_config_trait.h"

#include "loaded_config.hpp"

namespace eerie_leap::configuration::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::traits;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::utilities::cbor;

template <typename T>
class ConfigurationService {
private:
    const std::string configuration_dir_ = "config";

    std::string configuration_name_;
    std::shared_ptr<IFsService> fs_service_;
    std::shared_ptr<CborSerializer<T>> cbor_serializer_;

    const std::string configuration_file_path_ = configuration_dir_ + "/" + configuration_name_ + ".cbor";

    struct SaveTask {
        k_work work;
        ConfigurationService<T>* instance;
        T* configuration;
        bool result;
    };

    struct LoadTask {
        k_work work;
        ConfigurationService<T>* instance;
        std::optional<LoadedConfig<T>> result;
    };

    // NOTE: Save and Load perfomed on the System WorkQueue thread
    // to eliminates cases when configuration updated from some thread
    // wich will require that thread to have enough stack size for the operation.
    k_work_sync work_sync_;
    SaveTask task_save_;
    LoadTask task_load_;

    bool SaveProcessor(T* configuration) {
        LOG_MODULE_DECLARE(configuration_service_logger);

        auto config_bytes = cbor_serializer_->Serialize(*configuration);

        if (!config_bytes) {
            LOG_ERR("Failed to serialize configuration %s.", configuration_file_path_.c_str());
            return false;
        }

        return fs_service_->WriteFile(configuration_file_path_, config_bytes->data(), config_bytes->size());
    }

    std::optional<LoadedConfig<T>> LoadProcessor() {
        LOG_MODULE_DECLARE(configuration_service_logger);

        if (!fs_service_->Exists(configuration_file_path_)) {
            LOG_ERR("Configuration file %s does not exist.", configuration_file_path_.c_str());
            return std::nullopt;
        }

        size_t buffer_size = fs_service_->GetFileSize(configuration_file_path_);
        auto buffer = make_unique_ext<ExtVector>(buffer_size);
        size_t out_len = 0;

        if (!fs_service_->ReadFile(configuration_file_path_, buffer->data(), buffer_size, out_len)) {
            LOG_ERR("Failed to read configuration file %s.", configuration_file_path_.c_str());
            return std::nullopt;
        }

        auto config_bytes = std::span<const uint8_t>(buffer->data(), out_len);
        auto configuration = cbor_serializer_->Deserialize(config_bytes);

        if (configuration == nullptr) {
            LOG_ERR("Failed to deserialize configuration %s.", configuration_file_path_.c_str());
            return std::nullopt;
        }

        LoadedConfig<T> loaded_config {
            .config_raw = std::move(buffer),
            .config = std::move(configuration)
        };

        LOG_INF("%s configuration loaded successfully.", configuration_file_path_.c_str());

        return loaded_config;
    }

    static void WorkTaskSave(k_work* work) {
        SaveTask* task = CONTAINER_OF(work, SaveTask, work);

        task->result = task->instance->SaveProcessor(task->configuration);
    }

    static void WorkTaskLoad(k_work* work) {
        LoadTask* task = CONTAINER_OF(work, LoadTask, work);

        task->result = task->instance->LoadProcessor();
    }

public:
    ConfigurationService(std::string configuration_name, std::shared_ptr<IFsService> fs_service)
        : configuration_name_(std::move(configuration_name)), fs_service_(std::move(fs_service)) {

        task_save_.instance = this;
        k_work_init(&task_save_.work, WorkTaskSave);

        task_load_.instance = this;
        k_work_init(&task_load_.work, WorkTaskLoad);

        cbor_serializer_ = make_shared_ext<CborSerializer<T>>(
            CborTrait<T>::Encode, CborTrait<T>::Decode, CborTrait<T>::GetSerializingSize);

        if (!fs_service_->Exists(configuration_dir_))
            fs_service_->CreateDirectory(configuration_dir_);
    }

    bool Save(T* configuration) {
        task_save_.configuration = configuration;
        k_work_submit(&task_save_.work);
        k_work_flush(&task_save_.work, &work_sync_);

        return task_save_.result;
    }

    std::optional<LoadedConfig<T>> Load() {
        k_work_submit(&task_load_.work);
        k_work_flush(&task_load_.work, &work_sync_);

        return std::move(task_load_.result);
    }
};

} // namespace eerie_leap::configuration::services
