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
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller)
    : math_parser_service_(std::move(math_parser_service)),
    system_configuration_controller_(std::move(system_configuration_controller)),
    adc_configuration_controller_(std::move(adc_configuration_controller)),
    sensors_configuration_controller_(sensors_configuration_controller) {

        sensors_http_controller_ = make_shared_ext<SensorsApiController>(math_parser_service, sensors_configuration_controller);
    }

void HttpServer::Start() {
    int ret = http_server_start();
    if (ret != 0)
        LOG_ERR("Failed to start HTTP server, error: %d", ret);
    else
        LOG_INF("HTTP server started on port %u", http_service_port_);
}

HTTP_RESOURCE_DEFINE(sensors_config_resource, http_service, "/config/sensors", &SensorsApiController::sensors_config_resource_detail);

HTTP_RESOURCE_DEFINE(sensors_config_editor_html_resource, http_service, "/editor/sensors/index.html", &SensorsEditorController::sensors_config_editor_html_resource_detail);
HTTP_RESOURCE_DEFINE(sensors_config_editor_css_resource, http_service, "/editor/sensors/style.css", &SensorsEditorController::sensors_config_editor_css_resource_detail);
HTTP_RESOURCE_DEFINE(sensors_config_editor_js_resource, http_service, "/editor/sensors/editor.js", &SensorsEditorController::sensors_config_editor_js_resource_detail);
HTTP_RESOURCE_DEFINE(sensors_config_editor_prism_css_resource, http_service, "/editor/sensors/prism.css", &SensorsEditorController::sensors_config_editor_prism_css_resource_detail);
HTTP_RESOURCE_DEFINE(sensors_config_editor_prism_js_resource, http_service, "/editor/sensors/prism.js", &SensorsEditorController::sensors_config_editor_prism_js_resource_detail);


} // namespace eerie_leap::domain::http_domain::services
