#pragma once

#include <memory>
#include <span>

#include <zephyr/net/http/service.h>
#include <zephyr/data/json.h>

#include "utilities/memory/memory_resource_manager.h"
#include "configuration/json/configs/json_sensors_config.h"
#include "configuration/json/json_serializer.h"
#include "domain/sensor_domain/configuration/parsers/sensors_json_parser.h"
#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"
#include "domain/sensor_domain/services/processing_scheduler_service.h"

namespace eerie_leap::domain::http_domain::controllers::api {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::configuration::json::configs;
using namespace eerie_leap::configuration::json;

using namespace eerie_leap::domain::sensor_domain::configuration::parsers;
using namespace eerie_leap::domain::sensor_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::services;

class SensorsApiController {
private:
    static constexpr size_t sensors_config_post_buffer_size_ = 24576;

    static std::pmr::vector<uint8_t> sensors_config_post_buffer_;
    static std::pmr::string sensors_config_get_buffer_;

    static std::unique_ptr<SensorsJsonParser> sensors_json_parser_;
    static std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager_;
    static std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service_;
    static std::unique_ptr<JsonSerializer<JsonSensorsConfig>> json_serializer_;

    static void UpdateSensorsConfig(const std::span<const uint8_t>& buffer);

    static int sensors_config_get_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data);
    static int sensors_config_post_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data);
    static int sensors_config_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data);

public:
    static http_resource_detail_dynamic sensors_config_resource_detail;

    SensorsApiController(
        std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
        std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service);
};

} // namespace eerie_leap::domain::http_domain::controllers::api
