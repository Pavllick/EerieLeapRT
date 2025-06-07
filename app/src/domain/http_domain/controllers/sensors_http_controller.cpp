#include "utilities/memory/heap_allocator.hpp"
#include "sensors_http_controller.h"

namespace eerie_leap::domain::http_domain::controllers {

using namespace eerie_leap::utilities::memory;

std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller_ = nullptr;

SensorsHttpController::SensorsHttpController(std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller) {
    sensors_configuration_controller_ = std::move(sensors_configuration_controller);
}

std::shared_ptr<ExtVector> sensors_config_get_buffer_ = nullptr;
int SensorsHttpController::sensors_config_get_handler(struct http_client_ctx *client, enum http_data_status status, const struct http_request_ctx *request_ctx, struct http_response_ctx *response_ctx, void *user_data) {
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

        // ssize_t encoded_size = json_calc_encoded_len(sensors_descr, ARRAY_SIZE(sensors_descr), &data);
        size_t encoded_size = json_calc_encoded_arr_len(sensors_descr, &data);
        // HACK: Otherwise final JSON is being truncated by 1 byte
        sensors_config_get_buffer_ = make_shared_ext<ExtVector>(encoded_size + 1);

        // json_obj_encode_buf(sensors_descr, ARRAY_SIZE(sensors_descr), &data, (char*)sensors_config_get_buffer_->data(), sensors_config_get_buffer_->size());
        json_arr_encode_buf(sensors_descr, &data, (char*)sensors_config_get_buffer_->data(), sensors_config_get_buffer_->size());

        response_ctx->body = (uint8_t*)sensors_config_get_buffer_->data();
        response_ctx->body_len = encoded_size;
        response_ctx->final_chunk = true;
    }

    return 0;
}

http_resource_detail_dynamic SensorsHttpController::sensors_config_get_resource_detail = {
    .common = {
        .bitmask_of_supported_http_methods = BIT(HTTP_GET),
        .type = HTTP_RESOURCE_TYPE_DYNAMIC,
        .content_type = "application/json",
    },
    .cb = sensors_config_get_handler,
    .user_data = nullptr,
};

} // namespace eerie_leap::domain::http_domain::controllers
