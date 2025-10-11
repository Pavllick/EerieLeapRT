#pragma once

#include <memory>

#include "domain/system_domain/configuration/system_configuration_manager.h"
#include "domain/sensor_domain/configuration/adc_configuration_manager.h"
#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"
#include "domain/sensor_domain/services/processing_scheduler_service.h"

static constexpr uint16_t http_service_port_ = 8080;

namespace eerie_leap::domain::http_domain::services {

using namespace eerie_leap::domain::system_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::services;

class HttpServer {
private:
    std::shared_ptr<MathParserService> math_parser_service_;
    std::shared_ptr<SystemConfigurationManager> system_configuration_manager_;
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager_;
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager_;
    std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service_;

public:
    HttpServer() = default;
    ~HttpServer() = default;

    void Initialize(
        std::shared_ptr<MathParserService> math_parser_service,
        std::shared_ptr<SystemConfigurationManager> system_configuration_manager,
        std::shared_ptr<AdcConfigurationManager> adc_configuration_manager,
        std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
        std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service);
    void Start();
};

} // namespace eerie_leap::domain::http_domain::services
