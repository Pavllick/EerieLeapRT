#include <vector>
#include <zephyr/logging/log.h>

#include "utilities/memory/heap_allocator.hpp"
#include "measurement_service.h"
#include "domain/adc_domain/hardware/adc.h"
#include "domain/adc_domain/hardware/adc_emulator.h"

#include "domain/sensor_domain/utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "domain/sensor_domain/utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::adc_domain::hardware;
using namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator;

LOG_MODULE_REGISTER(measurement_service_logger);

k_tid_t MeasurementService::Start() {
    thread_id_ = k_thread_create(
        &thread_data_,
        stack_area_,
        K_THREAD_STACK_SIZEOF(stack_area_),
        [](void* instance, void* p2, void* p3) { static_cast<MeasurementService*>(instance)->EntryPoint(); },
        this, NULL, NULL,
        kPriority, 0, K_NO_WAIT);

    return thread_id_;
}

void MeasurementService::SetupTestSensors() {
    // Test Sensors

    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {3.3, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);

    ExpressionEvaluator expression_evaluator_1(math_parser_service_, "{x} * 2 + {sensor_2} + 1");

    Sensor sensor_1 {
        .id = "sensor_1",
        .metadata = {
            .name = "Sensor 1",
            .unit = "km/h",
            .description = "Test Sensor 1"
        },
        .configuration = {
            .type = SensorType::PHYSICAL_ANALOG,
            .channel = 0,
            .sampling_rate_ms = 1000,
            .voltage_interpolator = std::make_shared<LinearVoltageInterpolator>(calibration_data_1_ptr),
            .expression_evaluator = std::make_shared<ExpressionEvaluator>(expression_evaluator_1)
        }
    };

    std::vector<CalibrationData> calibration_data_2 {
        {0.0, 0.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_2);

    ExpressionEvaluator expression_evaluator_2(math_parser_service_, "x * 4 + 1.6");

    Sensor sensor_2 {
        .id = "sensor_2",
        .metadata = {
            .name = "Sensor 2",
            .unit = "km/h",
            .description = "Test Sensor 2"
        },
        .configuration = {
            .type = SensorType::PHYSICAL_ANALOG,
            .channel = 1,
            .sampling_rate_ms = 500,
            .voltage_interpolator = std::make_shared<CubicSplineVoltageInterpolator>(calibration_data_2_ptr),
            .expression_evaluator = std::make_shared<ExpressionEvaluator>(expression_evaluator_2)
        }
    };

    ExpressionEvaluator expression_evaluator_3(math_parser_service_, "{sensor_1} + 8.34");

    Sensor sensor_3 {
        .id = "sensor_3",
        .metadata = {
            .name = "Sensor 3",
            .unit = "km/h",
            .description = "Test Sensor 3"
        },
        .configuration = {
            .type = SensorType::VIRTUAL_ANALOG,
            .sampling_rate_ms = 2000,
            .expression_evaluator = std::make_shared<ExpressionEvaluator>(expression_evaluator_3)
        }
    };

    std::vector<std::shared_ptr<Sensor>> sensors = {
        std::make_shared<Sensor>(sensor_1),
        std::make_shared<Sensor>(sensor_2),
        std::make_shared<Sensor>(sensor_3)
    };

    auto res = sensors_configuration_controller_->Update(std::make_shared<std::vector<std::shared_ptr<Sensor>>>(sensors));
    if(!res)
        throw std::runtime_error("Cannot save config");
}

void MeasurementService::EntryPoint() {
    LOG_INF("Measurement Service started");

#ifdef CONFIG_ADC_EMUL
    adc_ = std::make_shared<AdcEmulator>();
#else
    adc_ = std::make_shared<Adc>();
#endif
    adc_->UpdateConfiguration(AdcConfiguration{
        .samples = 1
    });
    adc_->Initialize();

    math_parser_service_ = make_shared_ext<MathParserService>();

    sensors_configuration_controller_->Initialize(math_parser_service_);
    SetupTestSensors();

    processing_scheduler_service_ = std::make_shared<ProcessingSchedulerService>(time_service_, guid_generator_, adc_, sensors_configuration_controller_);

    processing_scheduler_service_->Start();

    return;
}

} // namespace eerie_leap::domain::sensor_domain::services
