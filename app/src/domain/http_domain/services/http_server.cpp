#include <zephyr/logging/log.h>
#include <zephyr/net/http/server.h>
#include <zephyr/net/http/service.h>

#include "domain/http_domain/controllers/sensors_http_controller.h"
#include "http_server.h"

HTTP_SERVICE_DEFINE(http_service, "0.0.0.0", &http_service_port_, 1, 10, nullptr, nullptr);

namespace eerie_leap::domain::http_domain::services {

using namespace eerie_leap::domain::http_domain::controllers;

LOG_MODULE_REGISTER(http_server);

static std::shared_ptr<SensorsHttpController> sensors_http_controller_;

HttpServer::HttpServer(
    std::shared_ptr<SystemConfigurationController> system_configuration_controller,
    std::shared_ptr<AdcConfigurationController> adc_configuration_controller,
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller)
    : system_configuration_controller_(std::move(system_configuration_controller)),
    adc_configuration_controller_(std::move(adc_configuration_controller)),
    sensors_configuration_controller_(sensors_configuration_controller) {

        sensors_http_controller_ = make_shared_ext<SensorsHttpController>(sensors_configuration_controller);
    }

void HttpServer::Start() {
    int ret = http_server_start();
    if (ret != 0)
        LOG_ERR("Failed to start HTTP server, error: %d", ret);
    else
        LOG_INF("HTTP server started on port %u", http_service_port_);
}

HTTP_RESOURCE_DEFINE(sensors_config_get_resource, http_service, "/config/sensors_config", &SensorsHttpController::sensors_config_get_resource_detail);

} // namespace eerie_leap::domain::http_domain::services
