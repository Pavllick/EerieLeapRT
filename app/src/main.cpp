#include <memory>
#include <ctime>
#include <zephyr/arch/cpu.h>
#include <zephyr/logging/log.h>

#include "domain/sensor_domain/models/sensor_type.h"
#include "utilities/memory/heap_allocator.h"
#include "utilities/dev_tools/system_info.h"
#include "utilities/guid/guid_generator.h"
#include "utilities/time/boot_elapsed_time_service.h"
#include "utilities/math_parser/math_parser_service.hpp"
#include "domain/fs_domain/services/fs_service.h"
#include "domain/hardware/gpio_domain/gpio_factory.hpp"
#include "domain/sensor_domain/services/processing_scheduler_serivce.h"
#include "domain/sensor_domain/services/calibration_service.h"

#include "configuration/system_config/system_config.h"
#include "configuration/adc_config/adc_config.h"
#include "configuration/sensor_config/sensor_config.h"
#include "configuration/services/configuration_service.h"
#include "controllers/sensors_configuration_controller.h"
#include "controllers/adc_configuration_controller.h"
#include "controllers/system_configuration_controller.h"

// Test sensors includes
#include "utilities/math_parser/expression_evaluator.h"
#include "domain/sensor_domain/models/sensor.h"

#include "utilities/voltage_interpolator/calibration_data.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::domain::sensor_domain::models;
// End test sensors

#ifdef CONFIG_WIFI
#include "domain/http_domain/services/wifi_ap_service.h"
#endif // CONFIG_WIFI

#ifdef CONFIG_NETWORKING
#include "domain/http_domain/services/http_server.h"
#endif // CONFIG_NETWORKING

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::utilities::dev_tools;
using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::utilities::math_parser;

using namespace eerie_leap::controllers;

using namespace eerie_leap::domain::hardware::adc_domain;
using namespace eerie_leap::domain::hardware::gpio_domain;
using namespace eerie_leap::domain::sensor_domain::services;
using namespace eerie_leap::domain::fs_domain::services;
using namespace eerie_leap::configuration::services;

#if defined(CONFIG_WIFI) || defined(CONFIG_NETWORKING)
using namespace eerie_leap::domain::http_domain::services;
#endif // CONFIG_WIFI || CONFIG_NETWORKING

LOG_MODULE_REGISTER(main_logger);

constexpr uint32_t SLEEP_TIME_MS = 10000;

void SetupAdcConfiguration(std::shared_ptr<AdcConfigurationController> adc_configuration_controller);
void SetupTestSensors(std::shared_ptr<MathParserService> math_parser_service, std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller);

// NOTE: ADC reading timing for ESP32S3:
// Each sample adds up to about 25us to the reading time

// NOTE: Sensor reading timing for ESP32S3:
// ADC reading with 40 samples and no expression evaluation takes around 1200us
// ADC reading with 40 samples and expression evaluation takes minimum of 3300us
// GPIO reading with no expression evaluation takes around 130us
// GPIO reading with expression evaluation takes minimum of 1900us
// Formula evaluation takes minimum of 1800us, applicable to virtual sensors

int main(void) {
    // NOTE: Don't use for WiFi supporting boards as WiFi is broken in Zephyr 4.1 and has memory allocation issues
    // At least on ESP32S3, it does connect if Zephyr revision is set to "main", but heap allocations cannot be moved
    // to the external RAM (e.g. PSRAM)
#ifdef CONFIG_WIFI
    WifiApService::Initialize();
#endif // CONFIG_WIFI

    // The HTTP Server started here, but objects it uses are initialized later
    // This is done to prevent memory allocation issues on boards with WiFi support
    // at least on the ESP32S3 otherwise it struggles sharing memory between http_server_tid and conn_mgr_monitor threads
    // Or extra heap can be allocated to connection manager through CONFIG_NET_CONNECTION_MANAGER_MONITOR_STACK_SIZE
    // Which also helps with the issue
#ifdef CONFIG_NETWORKING
    HttpServer http_server;
    http_server.Start();
#endif // CONFIG_NETWORKING

    auto fs_service = make_shared_ext<FsService>();

    auto time_service = make_shared_ext<BootElapsedTimeService>();
    time_service->Initialize();

    auto guid_generator = make_shared_ext<GuidGenerator>();
    auto math_parser_service = make_shared_ext<MathParserService>();

    auto system_config_service = make_shared_ext<ConfigurationService<SystemConfig>>("system_config", fs_service);
    auto adc_config_service = make_shared_ext<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto sensors_config_service = make_shared_ext<ConfigurationService<SensorsConfig>>("sensors_config", fs_service);

    auto adc_configuration_controller = make_shared_ext<AdcConfigurationController>(adc_config_service);

    // TODO: For test purposes only
    SetupAdcConfiguration(adc_configuration_controller);

    auto gpio = GpioFactory::Create();
    gpio->Initialize();

    auto system_configuration_controller = make_shared_ext<SystemConfigurationController>(system_config_service);
    auto sensors_configuration_controller = make_shared_ext<SensorsConfigurationController>(
        math_parser_service,
        sensors_config_service,
        adc_configuration_controller->Get()->GetChannelCount());

    // TODO: For test purposes only
    SetupTestSensors(math_parser_service, sensors_configuration_controller);

    auto processing_scheduler_service = make_shared_ext<ProcessingSchedulerService>(
        time_service,
        guid_generator,
        gpio,
        adc_configuration_controller,
        sensors_configuration_controller);

    auto calibration_service = make_shared_ext<CalibrationService>(
        time_service,
        guid_generator,
        adc_configuration_controller,
        processing_scheduler_service);

#ifdef CONFIG_NETWORKING
    http_server.Initialize(
        math_parser_service,
        system_configuration_controller,
        adc_configuration_controller,
        sensors_configuration_controller,
        processing_scheduler_service);
#endif // CONFIG_NETWORKING

    processing_scheduler_service->Start();

    // TODO: Remove this and integrate into http server
    // calibration_service->Start(1);
    // k_msleep(10000);
    // calibration_service->Stop();

    while (true) {
        SystemInfo::print_heap_info();
        SystemInfo::print_stack_info();
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}

void SetupAdcConfiguration(std::shared_ptr<AdcConfigurationController> adc_configuration_controller) {
    std::vector<CalibrationData> adc_calibration_data_samples {
        {0.501, 0.469},
        {1.0, 0.968},
        {2.0, 1.970},
        {3.002, 2.98},
        {4.003, 4.01},
        {5.0, 5.0}
    };

    auto adc_calibration_data_samples_ptr = make_shared_ext<std::vector<CalibrationData>>(adc_calibration_data_samples);
    auto adc_calibrator = make_shared_ext<AdcCalibrator>(InterpolationMethod::LINEAR, adc_calibration_data_samples_ptr);

    std::vector<std::shared_ptr<AdcChannelConfiguration>> channel_configurations = {
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
        make_shared_ext<AdcChannelConfiguration>(adc_calibrator),
    };

    auto adc_configuration = make_shared_ext<AdcConfiguration>();
    adc_configuration->samples = 40;
    adc_configuration->channel_configurations =
        make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    if(!adc_configuration_controller->Update(adc_configuration))
        throw std::runtime_error("Cannot save ADCs config");
}

void SetupTestSensors(std::shared_ptr<MathParserService> math_parser_service, std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller) {
    // Test Sensors

    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {5.0, 100.0}
    };
    auto calibration_data_1_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_data_1);

    // ExpressionEvaluator expression_evaluator_1(math_parser_service, "{x} * 2 + {sensor_2} + 1");

    Sensor sensor_1 {
        .id = "sensor_1",
        .metadata = {
            .name = "Sensor 1",
            .unit = "km/h",
            .description = "Test Sensor 1"
        },
        .configuration = {
            .type = SensorType::PHYSICAL_ANALOG,
            .channel = 1,
            .sampling_rate_ms = 250,
            .voltage_interpolator = make_shared_ext<LinearVoltageInterpolator>(calibration_data_1_ptr),
            // .expression_evaluator = make_shared_ext<ExpressionEvaluator>(expression_evaluator_1)
        }
    };

    std::vector<CalibrationData> calibration_data_2 {
        {0.0, 0.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_data_2);

    ExpressionEvaluator expression_evaluator_2(math_parser_service, "x * 4 + 1.6");

    Sensor sensor_2 {
        .id = "sensor_2",
        .metadata = {
            .name = "Sensor 2",
            .unit = "km/h",
            .description = "Test Sensor 2"
        },
        .configuration = {
            .type = SensorType::PHYSICAL_ANALOG,
            .channel = 6,
            .sampling_rate_ms = 500,
            .voltage_interpolator = make_shared_ext<CubicSplineVoltageInterpolator>(calibration_data_2_ptr),
            .expression_evaluator = make_shared_ext<ExpressionEvaluator>(expression_evaluator_2)
        }
    };

    ExpressionEvaluator expression_evaluator_3(math_parser_service, "{sensor_1} + 8.34");

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
            .expression_evaluator = make_shared_ext<ExpressionEvaluator>(expression_evaluator_3)
        }
    };

    Sensor sensor_4 {
        .id = "sensor_4",
        .metadata = {
            .name = "Sensor 4",
            .unit = "",
            .description = "Test Sensor 4"
        },
        .configuration = {
            .type = SensorType::PHYSICAL_INDICATOR,
            .channel = 1,
            .sampling_rate_ms = 1000
        }
    };

    ExpressionEvaluator expression_evaluator_5(math_parser_service, "{sensor_1} < 400");

    Sensor sensor_5 {
        .id = "sensor_5",
        .metadata = {
            .name = "Sensor 5",
            .unit = "",
            .description = "Test Sensor 5"
        },
        .configuration = {
            .type = SensorType::VIRTUAL_INDICATOR,
            .sampling_rate_ms = 1000,
            .expression_evaluator = make_shared_ext<ExpressionEvaluator>(expression_evaluator_5)
        }
    };

    std::vector<std::shared_ptr<Sensor>> sensors = {
        make_shared_ext<Sensor>(sensor_1),
        make_shared_ext<Sensor>(sensor_2),
        make_shared_ext<Sensor>(sensor_3),
        make_shared_ext<Sensor>(sensor_4),
        make_shared_ext<Sensor>(sensor_5)
    };

    auto sensors_ptr = make_shared_ext<std::vector<std::shared_ptr<Sensor>>>(sensors);
    auto res = sensors_configuration_controller->Update(sensors_ptr);
    if(!res)
        throw std::runtime_error("Cannot save Sensors config");
}
