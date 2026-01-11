#include <utility>

#include <zephyr/logging/log.h>

#include "sensors_configuration_manager.h"

namespace eerie_leap::domain::sensor_domain::configuration {

LOG_MODULE_REGISTER(sensors_config_ctrl_logger);

SensorsConfigurationManager::SensorsConfigurationManager(
    std::shared_ptr<IFsService> sd_fs_service,
    std::unique_ptr<CborConfigurationService<CborSensorsConfig>> cbor_configuration_service,
    std::unique_ptr<JsonConfigurationService<JsonSensorsConfig>> json_configuration_service,
    int gpio_channel_count,
    int adc_channel_count)
        : sd_fs_service_(std::move(sd_fs_service)),
        cbor_configuration_service_(std::move(cbor_configuration_service)),
        json_configuration_service_(std::move(json_configuration_service)),
        gpio_channel_count_(gpio_channel_count),
        adc_channel_count_(adc_channel_count) {

    cbor_parser_ = std::make_unique<SensorsCborParser>(sd_fs_service_);
    json_parser_ = std::make_unique<SensorsJsonParser>(sd_fs_service_);

    const std::vector<std::shared_ptr<Sensor>>* sensors = nullptr;

    try {
        sensors = Get(true);
    } catch(...) {
        LOG_ERR("Failed to load Sensors configuration.");
    }

    if(sensors == nullptr) {
        LOG_ERR("Failed to load Sensors configuration.");

        if(!CreateDefaultConfiguration()) {
            LOG_ERR("Failed to create default sensors configuration.");
            return;
        }

        LOG_INF("Default Sensors configuration loaded successfully.");
    } else {
        LOG_INF("Sensors Configuration Manager initialized successfully.");
    }

    ApplyJsonConfiguration();
}

bool SensorsConfigurationManager::ApplyJsonConfiguration() {
    if(!json_configuration_service_->IsAvailable())
        return false;

    auto json_config_loaded = json_configuration_service_->Load();
    if(json_config_loaded.has_value()) {
        if(json_config_loaded->checksum == json_config_checksum_)
            return true;

        try {
            auto sensors = json_parser_->Deserialize(
                Mrm::GetExtPmr(),
                *json_config_loaded->config,
                gpio_channel_count_,
                adc_channel_count_);

            json_config_checksum_ = json_config_loaded->checksum;

            if(!Update(sensors, true))
                return false;
        } catch(const std::exception& e) {
            LOG_ERR("Failed to deserialize JSON configuration. %s", e.what());
            return false;
        }

        LOG_INF("JSON configuration loaded successfully.");

        return true;
    }

    return true;
}

bool SensorsConfigurationManager::Update(const std::vector<std::shared_ptr<Sensor>>& sensors, bool internal_only) {
    try {
        if(!internal_only && json_configuration_service_->IsAvailable()) {
            auto json_config = json_parser_->Serialize(
                sensors,
                gpio_channel_count_,
                adc_channel_count_);
            json_configuration_service_->Save(json_config.get());

            auto json_config_loaded = json_configuration_service_->Load();
            if(!json_config_loaded.has_value()) {
                LOG_ERR("Failed to load newly updated JSON configuration.");
                return false;
            }

            LOG_INF("JSON configuration updated successfully.");

            json_config_checksum_ = json_config_loaded->checksum;
        }

        auto cbor_config = cbor_parser_->Serialize(
            sensors,
            gpio_channel_count_,
            adc_channel_count_);
        cbor_config->json_config_checksum = json_config_checksum_;

        if(!cbor_configuration_service_->Save(cbor_config.get()))
            return false;
    } catch(const std::exception& e) {
        LOG_ERR("Failed to update Sensors configuration. %s", e.what());
        return false;
    }

    return Get(true) != nullptr;
}

const std::vector<std::shared_ptr<Sensor>>* SensorsConfigurationManager::Get(bool force_load) {
    if(!sensors_.empty() && !force_load)
        return &sensors_;

    auto cbor_config_data = cbor_configuration_service_->Load();
    if(!cbor_config_data.has_value())
        return nullptr;

    auto cbor_config = std::move(cbor_config_data.value().config);

    sensors_.clear();
    sensors_ = cbor_parser_->Deserialize(
        Mrm::GetExtPmr(), *cbor_config, gpio_channel_count_, adc_channel_count_);

    json_config_checksum_ = cbor_config->json_config_checksum;

    return &sensors_;
}

bool SensorsConfigurationManager::CreateDefaultConfiguration() {
    auto sensors = std::vector<std::shared_ptr<Sensor>>();

    return Update(sensors);
}

} // namespace eerie_leap::domain::sensor_domain::configuration
