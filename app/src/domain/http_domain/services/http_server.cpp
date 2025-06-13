#include <zephyr/logging/log.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>

#include "domain/http_domain/controllers/api/sensors_api_controller.h"
#include "domain/http_domain/controllers/view/sensors_editor_controller.h"
#include "http_server.h"

HTTP_SERVICE_DEFINE(http_service, "0.0.0.0", &http_service_port_, 1, 10, nullptr, nullptr);

namespace eerie_leap::domain::http_domain::services {

using namespace eerie_leap::domain::http_domain::controllers::api;
using namespace eerie_leap::domain::http_domain::controllers::view;

LOG_MODULE_REGISTER(http_server);

static std::shared_ptr<SensorsApiController> sensors_http_controller_;

HttpServer::HttpServer(
    std::shared_ptr<MathParserService> math_parser_service,
    std::shared_ptr<SystemConfigurationController> system_configuration_controller,
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller,
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller,
    std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service)
    : math_parser_service_(std::move(math_parser_service)),
    system_configuration_controller_(std::move(system_configuration_controller)),
    adc_configuration_controller_(std::move(adc_configuration_controller)),
    sensors_configuration_controller_(sensors_configuration_controller),
    processing_scheduler_service_(processing_scheduler_service) {

        sensors_http_controller_ = make_shared_ext<SensorsApiController>(math_parser_service, sensors_configuration_controller, processing_scheduler_service);
    }

void HttpServer::Start() {
    int ret = http_server_start();
    if (ret != 0)
        LOG_ERR("Failed to start HTTP server, error: %d", ret);
    else
        LOG_INF("HTTP server started on port %u", http_service_port_);
}

// API Resources
HTTP_RESOURCE_DEFINE(sensors_config_resource, http_service, "/api/sensors/config", &SensorsApiController::sensors_config_resource_detail);

// View Resources
HTTP_RESOURCE_DEFINE(sensors_config_editor_html_resource, http_service, "/sensors/editor", &SensorsEditorController::sensors_config_editor_html_resource_detail);

} // namespace eerie_leap::domain::http_domain::services
