#include "domain/sensor_domain/utilities/voltage_interpolator/interpolation_method.h"
#include "utilities/memory/heap_allocator.hpp"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/utilities/voltage_interpolator/interpolation_method.h"
#include "domain/sensor_domain/utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "domain/sensor_domain/utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"
#include "sensors_api_controller.h"

namespace eerie_leap::domain::http_domain::controllers::api {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator;

std::shared_ptr<ExtVector> SensorsApiController::sensors_config_post_buffer_;
std::shared_ptr<ExtVector> SensorsApiController::sensors_config_get_buffer_;

std::shared_ptr<MathParserService> SensorsApiController::math_parser_service_ = nullptr;
std::shared_ptr<SensorsConfigurationController> SensorsApiController::sensors_configuration_controller_ = nullptr;

SensorsApiController::SensorsApiController(std::shared_ptr<MathParserService> math_parser_service, std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller) {
    sensors_config_post_buffer_ = make_shared_ext<ExtVector>(sensors_config_post_buffer_size_);

    math_parser_service_ = std::move(math_parser_service);
    sensors_configuration_controller_ = std::move(sensors_configuration_controller);
}

int SensorsApiController::sensors_config_get_handler(http_client_ctx *client, enum http_data_status status, const http_request_ctx *request_ctx, http_response_ctx *response_ctx, void *user_data) {
    if (status == HTTP_SERVER_DATA_ABORTED) {
        return 0;
    }

    if (status == HTTP_SERVER_DATA_FINAL) {
        auto sensors_configuration = sensors_configuration_controller_->Get();

        std::vector<SensorJsonDto> sensors;
        for(const auto& sensor : *sensors_configuration) {
            InterpolationMethod interpolation_method = sensor->configuration.voltage_interpolator != nullptr
                ? sensor->configuration.voltage_interpolator->GetInterpolationMethod()
                : InterpolationMethod::NONE;

            std::vector<SensorCalibrationDataMapJsonDto> calibration_table = {};
            if(interpolation_method != InterpolationMethod::NONE) {
                for(const auto& calibration_data : *sensor->configuration.voltage_interpolator->GetCalibrationTable()) {
                    calibration_table.push_back({
                        .voltage = calibration_data.voltage,
                        .value = calibration_data.value
                    });
                }
            }

            SensorJsonDto sensor_json {
                .id = sensor->id.c_str(),
                .metadata = {
                    .name = sensor->metadata.name.c_str(),
                    .unit = sensor->metadata.unit.c_str(),
                    .description = sensor->metadata.description.c_str()
                },
                .configuration = {
                    .type = GetSensorTypeName(sensor->configuration.type).c_str(),
                    .channel = sensor->configuration.channel.value_or(0),
                    .sampling_rate_ms = sensor->configuration.sampling_rate_ms,
                    .interpolation_method = GetInterpolationMethodName(interpolation_method).c_str(),
                    .calibration_table_len = calibration_table.size(),
                    .expression = sensor->configuration.expression_evaluator != nullptr
                        ? sensor->configuration.expression_evaluator->GetRawExpression()->c_str()
                        : ""
                }
            };

            for(size_t i = 0; i < calibration_table.size(); ++i)
                sensor_json.configuration.calibration_table[i] = calibration_table[i];

            sensors.push_back(sensor_json);
        }

        SensorsJsonDto data {
            .sensors_len = sensors.size()
        };
        for(size_t i = 0; i < sensors.size(); ++i)
            data.sensors[i] = sensors[i];

        ssize_t encoded_size = json_calc_encoded_len(sensors_descr, ARRAY_SIZE(sensors_descr), &data);
        // size_t encoded_size = json_calc_encoded_arr_len(sensors_descr, &data);
        // HACK: Otherwise final JSON is being truncated by 1 byte
        sensors_config_get_buffer_ = make_shared_ext<ExtVector>(encoded_size + 1);

        json_obj_encode_buf(sensors_descr, ARRAY_SIZE(sensors_descr), &data, (char*)sensors_config_get_buffer_->data(), sensors_config_get_buffer_->size());
        // json_arr_encode_buf(sensors_descr, &data, (char*)sensors_config_get_buffer_->data(), sensors_config_get_buffer_->size());

        response_ctx->body = (uint8_t*)sensors_config_get_buffer_->data();
        response_ctx->body_len = encoded_size;
        response_ctx->final_chunk = true;
    }

    return 0;
}

void SensorsApiController::ParseSensorsConfigJson(uint8_t *buffer, size_t len)
{
	int ret;
	SensorsJsonDto data;
	const int expected_return_code = BIT_MASK(ARRAY_SIZE(sensors_descr));

	ret = json_obj_parse((char*)buffer, len, sensors_descr, ARRAY_SIZE(sensors_descr), &data);
	if (ret != expected_return_code)
        throw std::runtime_error("Invalid JSON payload.");

    printk("JSON payload parsed successfully\n");

    std::vector<std::shared_ptr<Sensor>> sensors;

    for(size_t i = 0; i < data.sensors_len; ++i) {
        auto sensor = make_shared_ext<Sensor>();
        sensor->id = data.sensors[i].id;

        sensor->metadata.unit = data.sensors[i].metadata.unit;
        sensor->metadata.name = data.sensors[i].metadata.name;
        sensor->metadata.description = data.sensors[i].metadata.description;

        sensor->configuration.type = GetSensorType(data.sensors[i].configuration.type);
        sensor->configuration.channel = data.sensors[i].configuration.channel;
        sensor->configuration.sampling_rate_ms = data.sensors[i].configuration.sampling_rate_ms;

        auto interpolation_method = GetInterpolationMethod(data.sensors[i].configuration.interpolation_method);
        if(interpolation_method != InterpolationMethod::NONE && data.sensors[i].configuration.calibration_table_len > 0) {
            std::vector<CalibrationData> calibration_table;
            for(size_t j = 0; j < data.sensors[i].configuration.calibration_table_len; ++j) {
                const auto& calibration_data = data.sensors[i].configuration.calibration_table[j];

                calibration_table.push_back({
                    .voltage = calibration_data.voltage,
                    .value = calibration_data.value});
            }

            auto calibration_table_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_table);

            switch (interpolation_method) {
            case InterpolationMethod::LINEAR:
                sensor->configuration.voltage_interpolator = make_shared_ext<LinearVoltageInterpolator>(calibration_table_ptr);
                break;

            case InterpolationMethod::CUBIC_SPLINE:
                sensor->configuration.voltage_interpolator = make_shared_ext<CubicSplineVoltageInterpolator>(calibration_table_ptr);
                break;

            default:
                throw std::runtime_error("Sensor uses unsupported interpolation method.");
                break;
            }
        } else {
            sensor->configuration.voltage_interpolator = nullptr;
        }

        if(strcmp(data.sensors[i].configuration.expression, "") != 0)
            sensor->configuration.expression_evaluator = make_shared_ext<ExpressionEvaluator>(
                math_parser_service_,
                std::string(data.sensors[i].configuration.expression));
        else
            sensor->configuration.expression_evaluator = nullptr;

        sensors.push_back(sensor);
    }

    if(sensors_configuration_controller_->Update(make_shared_ext<std::vector<std::shared_ptr<Sensor>>>(sensors)))
        printk("Sensors configuration updated successfully\n");
    else
        throw std::runtime_error("Failed to update sensors configuration.");
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

    static std::shared_ptr<std::string> error_msg;

	if (status == HTTP_SERVER_DATA_FINAL) {
        printk("JSON payload received successfully, len=%zu\n", cursor);

        try {
            ParseSensorsConfigJson(sensors_config_post_buffer_->data(), cursor);
        } catch (const std::exception &e) {
            printk("Failed to parse JSON: %s\n", e.what());

            error_msg = make_shared_ext<std::string>("Failed to parse JSON: " + std::string(e.what()));

            response_ctx->status = HTTP_500_INTERNAL_SERVER_ERROR;
            response_ctx->body = (uint8_t*)error_msg->c_str();
            response_ctx->body_len = error_msg->size();
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
