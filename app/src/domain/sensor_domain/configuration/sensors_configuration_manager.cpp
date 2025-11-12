#include <utility>

#include <zephyr/logging/log.h>

#include "sensors_configuration_manager.h"

namespace eerie_leap::domain::sensor_domain::configuration {

LOG_MODULE_REGISTER(sensors_config_ctrl_logger);

SensorsConfigurationManager::SensorsConfigurationManager(
    std::shared_ptr<MathParserService> math_parser_service,
    ext_unique_ptr<CborConfigurationService<CborSensorsConfig>> cbor_configuration_service,
    int gpio_channel_count,
    int adc_channel_count)
        : math_parser_service_(std::move(math_parser_service)),
        cbor_configuration_service_(std::move(cbor_configuration_service)),
        config_(make_unique_ext<CborSensorsConfig>()),
        gpio_channel_count_(gpio_channel_count),
        adc_channel_count_(adc_channel_count) {

    sensors_cbor_parser_ = std::make_unique<SensorsCborParser>(math_parser_service_);

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
}

bool SensorsConfigurationManager::Update(const std::vector<std::shared_ptr<Sensor>>& sensors) {
    auto sensors_config =
        sensors_cbor_parser_->Serialize(sensors, gpio_channel_count_, adc_channel_count_);

    LOG_INF("Saving sensors configuration.");
    bool res = cbor_configuration_service_->Save(sensors_config.get());
    if(!res)
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

    config_raw_ = std::move(config.value().config_raw);
    config_ = std::move(config.value().config);

    sensors_ = sensors_cbor_parser_->Deserialize(
        *config_.get(), gpio_channel_count_, adc_channel_count_);

    return &sensors_;
}

const std::span<uint8_t> SensorsConfigurationManager::GetRaw() {
    return *config_raw_.get();
}

bool SensorsConfigurationManager::CreateDefaultConfiguration() {
    auto sensors = std::vector<std::shared_ptr<Sensor>>();

    return Update(sensors);
}

} // namespace eerie_leap::domain::sensor_domain::configuration
