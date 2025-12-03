#include <utility>
#include <zephyr/logging/log.h>

#include "utilities/voltage_interpolator/interpolation_method.h"
#include "utilities/memory/heap_allocator.h"
#include "configuration/json/traits/sensors_config_trait.h"

#include "sensors_api_controller.h"

namespace eerie_leap::domain::http_domain::controllers::api {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::configuration::json::traits;

LOG_MODULE_REGISTER(sensors_api_controller_logger);

const size_t SensorsApiController::sensors_config_post_buffer_size_;

ext_unique_ptr<ExtVector> SensorsApiController::sensors_config_post_buffer_;
std::unique_ptr<ExtString> SensorsApiController::sensors_config_get_buffer_;

std::unique_ptr<SensorsJsonParser> SensorsApiController::sensors_json_parser_ = nullptr;
std::shared_ptr<SensorsConfigurationManager> SensorsApiController::sensors_configuration_manager_ = nullptr;
std::shared_ptr<ProcessingSchedulerService> SensorsApiController::processing_scheduler_service_ = nullptr;
std::unique_ptr<JsonSerializer<JsonSensorsConfig>> SensorsApiController::json_serializer_ = nullptr;

SensorsApiController::SensorsApiController(
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
    std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service) {

    if(!sensors_config_post_buffer_)
        sensors_config_post_buffer_ = make_unique_ext<ExtVector>(sensors_config_post_buffer_size_);

    if(!sensors_json_parser_)
        sensors_json_parser_ = std::make_unique<SensorsJsonParser>(nullptr);

    if(!sensors_configuration_manager_)
        sensors_configuration_manager_ = std::move(sensors_configuration_manager);

    if(!processing_scheduler_service_)
        processing_scheduler_service_ = std::move(processing_scheduler_service);

    if(!json_serializer_)
        json_serializer_ = std::make_unique<JsonSerializer<JsonSensorsConfig>>(
            JsonTrait<JsonSensorsConfig>::Encode,
            JsonTrait<JsonSensorsConfig>::Decode);
}

int SensorsApiController::sensors_config_get_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data) {
    if (status == HTTP_SERVER_DATA_ABORTED)
        return 0;

    if (status == HTTP_SERVER_DATA_FINAL) {
        const auto* sensors = sensors_configuration_manager_->Get();
        // TODO: Get GPIO and ADC channel count
        auto config = sensors_json_parser_->Serialize(*sensors, 16, 16);
        sensors_config_get_buffer_ = json_serializer_->Serialize(*config);

        response_ctx->body = (uint8_t*)sensors_config_get_buffer_->c_str();
        response_ctx->body_len = sensors_config_get_buffer_->size();
        response_ctx->final_chunk = true;
    }

    return 0;
}

void SensorsApiController::UpdateSensorsConfig(const std::span<const uint8_t>& buffer) {
    // TODO: Get GPIO and ADC channel count
    std::string_view json_str(reinterpret_cast<const char*>(buffer.data()), buffer.size());
    auto json_sensors_config = json_serializer_->Deserialize(json_str);
    auto sensors = sensors_json_parser_->Deserialize(*json_sensors_config, 16, 16);

    if(!sensors_configuration_manager_->Update(sensors))
        throw std::runtime_error("Failed to update sensors configuration.");

    processing_scheduler_service_->Restart();
}

int SensorsApiController::sensors_config_post_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data) {
    static size_t cursor;

	if (status == HTTP_SERVER_DATA_ABORTED) {
		cursor = 0;
		return 0;
	}

	if (request_ctx->data_len + cursor > sensors_config_post_buffer_->size()) {
		cursor = 0;
		return -ENOMEM;
	}

	memcpy(sensors_config_post_buffer_->data() + cursor, request_ctx->data, request_ctx->data_len);
	cursor += request_ctx->data_len;

    static std::string error_msg;

	if (status == HTTP_SERVER_DATA_FINAL) {
        LOG_DBG("JSON payload received successfully, len=%zu", cursor);

        try {
            std::span<uint8_t> buffer(sensors_config_post_buffer_->data(), cursor);
            UpdateSensorsConfig(buffer);
        } catch (const std::exception &e) {
            LOG_ERR("Failed to parse JSON: %s", e.what());

            error_msg = "Failed to parse JSON: " + std::string(e.what());

            response_ctx->status = HTTP_500_INTERNAL_SERVER_ERROR;
            response_ctx->body = (uint8_t*)error_msg.c_str();
            response_ctx->body_len = error_msg.size();
            response_ctx->final_chunk = true;
        }

		cursor = 0;
	}

	return 0;
}

int SensorsApiController::sensors_config_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data) {
	if (client->method == HTTP_GET) {
		return sensors_config_get_handler(client, status, request_ctx, response_ctx, user_data);
	} else if (client->method == HTTP_POST) {
		return sensors_config_post_handler(client, status, request_ctx, response_ctx, user_data);
	}

    return -EINVAL;
}

http_resource_detail_dynamic SensorsApiController::sensors_config_resource_detail = {
	.common = {
        .bitmask_of_supported_http_methods = BIT(HTTP_GET) | BIT(HTTP_POST),
        .type = HTTP_RESOURCE_TYPE_DYNAMIC,
        .content_type = "application/json",
    },
	.cb = sensors_config_handler,
	.user_data = nullptr,
};

} // namespace eerie_leap::domain::http_domain::controllers
