#pragma once

#include <memory>

#include "controllers/system_configuration_controller.h"
#include "controllers/adc_configuration_controller.h"
#include "controllers/sensors_configuration_controller.h"

static constexpr uint16_t http_service_port_ = 8080;

namespace eerie_leap::domain::http_domain::services {

using namespace eerie_leap::controllers;

class HttpServer {
private:
    std::shared_ptr<MathParserService> math_parser_service_;
    std::shared_ptr<SystemConfigurationController> system_configuration_controller_;
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller_;
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller_;
public:
    HttpServer(
        std::shared_ptr<MathParserService> math_parser_service,
        std::shared_ptr<SystemConfigurationController> system_configuration_controller,
        std::shared_ptr<AdcConfigurationController> adc_configuration_controller,
        std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller);

    void Start();
};

} // namespace eerie_leap::domain::http_domain::services
