#include <utility>

#include <zephyr/logging/log.h>
#include <zephyr/sys/crc.h>

#include "sensors_configuration_manager.h"

namespace eerie_leap::domain::sensor_domain::configuration {

LOG_MODULE_REGISTER(sensors_config_ctrl_logger);

SensorsConfigurationManager::SensorsConfigurationManager(
    std::shared_ptr<MathParserService> math_parser_service,
    ext_unique_ptr<CborConfigurationService<CborSensorsConfig>> cbor_configuration_service,
    ext_unique_ptr<JsonConfigurationService<JsonSensorsConfig>> json_configuration_service,
    int gpio_channel_count,
    int adc_channel_count)
        : math_parser_service_(std::move(math_parser_service)),
        cbor_configuration_service_(std::move(cbor_configuration_service)),
        json_configuration_service_(std::move(json_configuration_service)),
        cbor_config_(make_unique_ext<CborSensorsConfig>()),
        gpio_channel_count_(gpio_channel_count),
        adc_channel_count_(adc_channel_count) {

    cbor_parser_ = std::make_unique<SensorsCborParser>(math_parser_service_);
    json_parser_ = std::make_unique<SensorsJsonParser>(math_parser_service_);

    const std::vector<std::shared_ptr<Sensor>>* sensors = nullptr;

    try {
        sensors = Get(true);
    } catch(const std::exception& e) {}

    if(sensors == nullptr) {
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
        uint32_t crc = crc32_ieee(json_config_loaded->config_raw->data(), json_config_loaded->config_raw->size());
        if(crc == json_config_checksum_)
            return true;

        auto sensors = json_parser_->Deserialize(
            *json_config_loaded->config,
            gpio_channel_count_,
            adc_channel_count_);

        bool result = Update(sensors);
        if(!result)
            return false;

        LOG_INF("JSON configuration loaded successfully.");

        return true;
    }

    return Update(sensors_);
}

bool SensorsConfigurationManager::Update(const std::vector<std::shared_ptr<Sensor>>& sensors) {
    if(json_configuration_service_->IsAvailable()) {
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

        uint32_t crc = crc32_ieee(json_config_loaded->config_raw->data(), json_config_loaded->config_raw->size());
        json_config_checksum_ = crc;
    }

    auto cbor_config = cbor_parser_->Serialize(
        sensors,
        gpio_channel_count_,
        adc_channel_count_);
    cbor_config->json_config_checksum = json_config_checksum_;

    if(!cbor_configuration_service_->Save(cbor_config.get()))
        return false;

    Get(true);

    return true;
}

const std::vector<std::shared_ptr<Sensor>>* SensorsConfigurationManager::Get(bool force_load) {
    if(!sensors_.empty() && !force_load)
        return &sensors_;

    auto config = cbor_configuration_service_->Load();
    if(!config.has_value())
        return nullptr;

    cbor_config_raw_ = std::move(config.value().config_raw);
    cbor_config_ = std::move(config.value().config);

    sensors_ = cbor_parser_->Deserialize(
        *cbor_config_.get(), gpio_channel_count_, adc_channel_count_);

    json_config_checksum_ = cbor_config_->json_config_checksum;

    return &sensors_;
}

bool SensorsConfigurationManager::CreateDefaultConfiguration() {
    auto sensors = std::vector<std::shared_ptr<Sensor>>();

    return Update(sensors);
}

} // namespace eerie_leap::domain::sensor_domain::configuration
