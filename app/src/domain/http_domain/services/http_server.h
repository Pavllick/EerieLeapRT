#pragma once

#include <memory>

#include "domain/system_domain/configuration/system_configuration_manager.h"
#include "domain/sensor_domain/configuration/adc_configuration_manager.h"
#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"
#include "domain/sensor_domain/services/sensors_processing_service.h"

static constexpr uint16_t http_service_port_ = 8080;

namespace eerie_leap::domain::http_domain::services {

using eerie_leap::domain::system_domain::configuration::SystemConfigurationManager;
using eerie_leap::domain::sensor_domain::configuration::AdcConfigurationManager;
using eerie_leap::domain::sensor_domain::configuration::SensorsConfigurationManager;
using eerie_leap::domain::sensor_domain::services::SensorsProcessingService;

class HttpServer {
private:
    std::shared_ptr<SystemConfigurationManager> system_configuration_manager_;
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager_;
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager_;
    std::shared_ptr<SensorsProcessingService> sensors_processing_service_;

public:
    HttpServer() = default;
    ~HttpServer() = default;

    void Initialize(
        std::shared_ptr<SystemConfigurationManager> system_configuration_manager,
        std::shared_ptr<AdcConfigurationManager> adc_configuration_manager,
        std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
        std::shared_ptr<SensorsProcessingService> sensors_processing_service);
    void Start();
};

} // namespace eerie_leap::domain::http_domain::services
