#pragma once

#include <memory>

#include "controllers/system_configuration_controller.h"
#include "controllers/adcs_configuration_controller.h"
#include "controllers/sensors_configuration_controller.h"
#include "domain/sensor_domain/services/processing_scheduler_serivce.h"

static constexpr uint16_t http_service_port_ = 8080;

namespace eerie_leap::domain::http_domain::services {

using namespace eerie_leap::controllers;
using namespace eerie_leap::domain::sensor_domain::services;

class HttpServer {
private:
    std::shared_ptr<MathParserService> math_parser_service_;
    std::shared_ptr<SystemConfigurationController> system_configuration_controller_;
    std::shared_ptr<AdcsConfigurationController> adcs_configuration_controller_;
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller_;
    std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service_;

public:
    HttpServer(
        std::shared_ptr<MathParserService> math_parser_service,
        std::shared_ptr<SystemConfigurationController> system_configuration_controller,
        std::shared_ptr<AdcsConfigurationController> adcs_configuration_controller,
        std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller,
        std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service);

    void Start();
};

} // namespace eerie_leap::domain::http_domain::services
