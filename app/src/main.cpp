#include <memory>
#include <zephyr/arch/cpu.h>
#include <zephyr/logging/log.h>

#include "utilities/memory/heap_allocator.h"
#include "utilities/dev_tools/system_info.h"
#include "utilities/guid/guid_generator.h"
#include "utilities/math_parser/math_parser_service.hpp"

#include "subsys/device_tree/dt_configurator.h"
#include "subsys/device_tree/dt_fs.h"
#include "subsys/device_tree/dt_modbus.h"
#include "subsys/device_tree/dt_canbus.h"
#include "subsys/device_tree/dt_display.h"

#include "subsys/fs/services/fs_service.h"
#include "subsys/fs/services/sdmmc_service.h"
#include "subsys/gpio/gpio_factory.hpp"
#include "subsys/modbus/modbus.h"
#include "subsys/cfb/cfb.h"
#include "subsys/canbus/canbus.h"
#include "subsys/dbc/dbc.h"
#include "subsys/time/time_service.h"
#include "subsys/time/rtc_provider.h"
#include "subsys/time/boot_elapsed_time_provider.h"

#include "configuration/system_config/system_config.h"
#include "configuration/adc_config/adc_config.h"
#include "configuration/sensor_config/sensor_config.h"
#include "configuration/services/configuration_service.h"
#include "domain/system_domain/configuration/system_configuration_manager.h"
#include "domain/sensor_domain/configuration/adc_configuration_manager.h"
#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"
#include "domain/sensor_domain/sensor_readers/sensor_reader_factory.h"
#include "domain/sensor_domain/services/processing_scheduler_service.h"
#include "domain/sensor_domain/services/calibration_service.h"

#include "domain/logging_domain/services/log_writer_service.h"

#include "domain/user_com_domain/user_com.h"
#include "domain/user_com_domain/services/com_polling/com_polling_interface_service.h"
#include "domain/user_com_domain/services/com_reading/com_reading_interface_service.h"

#include "controllers/logging_controller.h"
#include "controllers/com_polling_controller.h"
#include "controllers/display_controller.h"

// Test sensors includes
#include "utilities/math_parser/expression_evaluator.h"
#include "domain/sensor_domain/models/sensor.h"

#include "utilities/voltage_interpolator/calibration_data.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::domain::sensor_domain::configuration;
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
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::utilities::math_parser;

using namespace eerie_leap::subsys::device_tree;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::gpio;
using namespace eerie_leap::subsys::cfb;
using namespace eerie_leap::subsys::modbus;
using namespace eerie_leap::subsys::canbus;
using namespace eerie_leap::subsys::dbc;
using namespace eerie_leap::subsys::time;

using namespace eerie_leap::configuration::services;

using namespace eerie_leap::domain::system_domain::configuration;
using namespace eerie_leap::domain::sensor_domain::services;
using namespace eerie_leap::domain::sensor_domain::sensor_readers;
using namespace eerie_leap::domain::user_com_domain;
using namespace eerie_leap::domain::user_com_domain::services::com_reading;
using namespace eerie_leap::domain::user_com_domain::services::com_polling;
using namespace eerie_leap::domain::logging_domain::services;

using namespace eerie_leap::controllers;

#if defined(CONFIG_WIFI) || defined(CONFIG_NETWORKING)
using namespace eerie_leap::domain::http_domain::services;
#endif // CONFIG_WIFI || CONFIG_NETWORKING

LOG_MODULE_REGISTER(main_logger);

constexpr uint32_t SLEEP_TIME_MS = 10000;

void SetupAdcConfiguration(std::shared_ptr<AdcConfigurationManager> adc_configuration_manager);
void SetupTestSensors(std::shared_ptr<MathParserService> math_parser_service, std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager);

// NOTE: ADC reading timing for ESP32S3:
// Each sample adds up to about 25us to the reading time

// NOTE: Sensor reading timing for ESP32S3:
// ADC reading with 40 samples and no expression evaluation takes around 1200us
// ADC reading with 40 samples and expression evaluation takes minimum of 3300us
// GPIO reading with no expression evaluation takes around 130us
// GPIO reading with expression evaluation takes minimum of 1900us
// Formula evaluation takes minimum of 1800us, applicable to virtual sensors

int main(void) {
    DtConfigurator::Initialize();

    std::shared_ptr<Cfb> cfb = nullptr;

    if(DtDisplay::Get() != nullptr) {
        cfb = make_shared_ext<Cfb>();

    if(!cfb->Initialize()) {
        LOG_ERR("Failed to initialize CFB.");
        return -1;
    }
    }

    auto display_controller = make_shared_ext<DisplayController>(cfb);
    display_controller->Initialize();

    std::shared_ptr<SdmmcService> sd_fs_service = nullptr;
    auto sd_fs_mp = DtFs::GetSdFsMp();
    if(sd_fs_mp.has_value()) {
        sd_fs_service = make_shared_ext<SdmmcService>(sd_fs_mp.value(), DtFs::GetSdDiskName());
        if(!sd_fs_service->Initialize()) {
            LOG_ERR("Failed to initialize SD File System.");
        }

        // TODO: SPI SD card detection causes card to be
        // constatly mounted/umounted during logging
        // sd_fs_service->SdMonitorStart();
    }

    // NOTE: Don't use for WiFi supporting boards as WiFi is broken in Zephyr 4.1 and has memory allocation issues
    // At least on ESP32S3, it does connect if Zephyr revision is set to "main", but heap allocations cannot be moved
    // to the external RAM (e.g. PSRAM)

    // NOTE: Wifi thread seems to run out of memory on ESP32S3 after random period of time
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

    auto fs_service = make_shared_ext<FsService>(DtFs::GetInternalFsMp().value());
    if(!fs_service->Initialize()) {
        LOG_ERR("Failed to initialize File System.");
        return -1;
    }

    auto rtc_provider = make_shared_ext<RtcProvider>();
    auto boot_elapsed_time_provider = make_shared_ext<BootElapsedTimeProvider>();
    auto time_service = make_shared_ext<TimeService>(rtc_provider, boot_elapsed_time_provider);
    time_service->Initialize();

    auto guid_generator = make_shared_ext<GuidGenerator>();
    auto math_parser_service = make_shared_ext<MathParserService>();

    auto system_config_service = make_unique_ext<ConfigurationService<SystemConfig>>("system_config", fs_service);
    auto adc_config_service = make_unique_ext<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto sensors_config_service = make_unique_ext<ConfigurationService<SensorsConfig>>("sensors_config", fs_service);

    auto adc_configuration_manager = make_shared_ext<AdcConfigurationManager>(std::move(adc_config_service));

    // TODO: For test purposes only
    // SetupAdcConfiguration(adc_configuration_manager);

    auto gpio = GpioFactory::Create();
    gpio->Initialize();

    std::shared_ptr<Canbus> canbus = nullptr;
    if(DtCanbus::Get() != nullptr) {
        canbus = make_shared_ext<Canbus>(DtCanbus::Get());
        if(!canbus->Initialize()) {
            LOG_ERR("Failed to initialize CANBus.");
            return -1;
        }
    }

    auto dbc = make_shared_ext<Dbc>();

    auto system_configuration_manager = make_shared_ext<SystemConfigurationManager>(std::move(system_config_service));
    auto sensors_configuration_manager = make_shared_ext<SensorsConfigurationManager>(
        math_parser_service,
        std::move(sensors_config_service),
        adc_configuration_manager->Get()->GetChannelCount());

    // TODO: For test purposes only
    SetupTestSensors(math_parser_service, sensors_configuration_manager);

    auto sensor_readings_frame = make_shared_ext<SensorReadingsFrame>();

    std::shared_ptr<LoggingController> logging_controller = nullptr;
    if(sd_fs_service != nullptr) {
        auto log_writer_service = make_shared_ext<LogWriterService>(
            sd_fs_service,
            time_service,
            sensor_readings_frame);
        log_writer_service->Initialize();

        logging_controller = make_shared_ext<LoggingController>(
            log_writer_service,
            sensors_configuration_manager,
            display_controller);
    }

    std::shared_ptr<ComPollingInterfaceService> com_polling_interface_service = nullptr;
    std::shared_ptr<ComReadingInterfaceService> com_reading_interface_service = nullptr;
    std::shared_ptr<ComPollingController> com_polling_controller = nullptr;
    if(DtModbus::Get() != nullptr) {
        auto modbus = make_shared_ext<Modbus>(DtModbus::Get());
        auto user_com_interface = make_shared_ext<UserCom>(modbus, system_configuration_manager);
    user_com_interface->Initialize();

    // user_com_interface->ResolveUserIds();

    auto com_users = user_com_interface->GetUsers();
    LOG_INF("User IDs count: %zu", com_users->size());
    for(auto com_user : *com_users) {
        LOG_INF("Com User Device ID: %llu, Server ID: %hu", com_user.device_id, com_user.server_id);
    }

        com_polling_interface_service = make_shared_ext<ComPollingInterfaceService>(user_com_interface);
        com_polling_interface_service->Initialize();

        com_reading_interface_service = make_shared_ext<ComReadingInterfaceService>(user_com_interface, sensor_readings_frame);
        com_reading_interface_service->Initialize();
    
        if(logging_controller != nullptr) {
            com_polling_controller = make_shared_ext<ComPollingController>(user_com_interface, com_polling_interface_service, logging_controller);
            com_polling_controller->Initialize();
        }
    }

    auto sensor_reader_factory = make_shared_ext<SensorReaderFactory>(
        time_service,
        guid_generator,
        gpio,
        adc_configuration_manager,
        sensor_readings_frame,
        canbus,
        dbc);

    auto processing_scheduler_service = make_shared_ext<ProcessingSchedulerService>(
        sensors_configuration_manager,
        sensor_readings_frame,
        sensor_reader_factory);
    processing_scheduler_service->Initialize();

    auto calibration_service = make_shared_ext<CalibrationService>(
        time_service,
        guid_generator,
        adc_configuration_manager,
        processing_scheduler_service);

#ifdef CONFIG_NETWORKING
    http_server.Initialize(
        math_parser_service,
        system_configuration_manager,
        adc_configuration_manager,
        sensors_configuration_manager,
        processing_scheduler_service);
#endif // CONFIG_NETWORKING

    processing_scheduler_service->Start();

    // TODO: Remove this and integrate into http server
    // calibration_service->Start(1);
    // k_msleep(10000);
    // calibration_service->Stop();

    while(true) {
        SystemInfo::print_heap_info();
        SystemInfo::print_stack_info();
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}

void SetupAdcConfiguration(std::shared_ptr<AdcConfigurationManager> adc_configuration_manager) {
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

    if(!adc_configuration_manager->Update(*adc_configuration.get()))
        throw std::runtime_error("Cannot save ADCs config");
}

void SetupTestSensors(std::shared_ptr<MathParserService> math_parser_service, std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager) {
    // Test Sensors

    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {5.0, 100.0}
    };
    auto calibration_data_1_ptr = make_shared_ext<std::vector<CalibrationData>>(calibration_data_1);

    // ExpressionEvaluator expression_evaluator_1(math_parser_service, "{x} * 2 + {sensor_2} + 1");

    auto sensor_1 = make_shared_ext<Sensor>();
    sensor_1->id = "sensor_1";
    sensor_1->metadata = {
            .name = "Sensor 1",
            .unit = "km/h",
            .description = "Test Sensor 1"
    };
    sensor_1->configuration = {
            .type = SensorType::PHYSICAL_ANALOG,
            .channel = 0,
            .sampling_rate_ms = 10,
        .voltage_interpolator = make_unique_ext<LinearVoltageInterpolator>(calibration_data_1_ptr),
        // .expression_evaluator = make_unique_ext<ExpressionEvaluator>(expression_evaluator_1)
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

    auto sensor_2 = make_shared_ext<Sensor>();
    sensor_2->id = "sensor_2";
    sensor_2->metadata = {
            .name = "Sensor 2",
            .unit = "km/h",
            .description = "Test Sensor 2"
    };
    sensor_2->configuration = {
            .type = SensorType::PHYSICAL_ANALOG,
            .channel = 1,
        .sampling_rate_ms = 1000,
        .voltage_interpolator = make_unique_ext<CubicSplineVoltageInterpolator>(calibration_data_2_ptr),
        .expression_evaluator = make_unique_ext<ExpressionEvaluator>(expression_evaluator_2)
    };

    ExpressionEvaluator expression_evaluator_3(math_parser_service, "{sensor_1} + 8.34");

    auto sensor_3 = make_shared_ext<Sensor>();
    sensor_3->id = "sensor_3";
    sensor_3->metadata = {
            .name = "Sensor 3",
            .unit = "km/h",
            .description = "Test Sensor 3"
    };
    sensor_3->configuration = {
            .type = SensorType::VIRTUAL_ANALOG,
            .sampling_rate_ms = 2000,
        .expression_evaluator = make_unique_ext<ExpressionEvaluator>(expression_evaluator_3)
    };

    auto sensor_4 = make_shared_ext<Sensor>();
    sensor_4->id = "sensor_4";
    sensor_4->metadata = {
            .name = "Sensor 4",
            .unit = "",
            .description = "Test Sensor 4"
    };
    sensor_4->configuration = {
            .type = SensorType::PHYSICAL_INDICATOR,
            .channel = 1,
            .sampling_rate_ms = 1000
    };

    ExpressionEvaluator expression_evaluator_5(math_parser_service, "{sensor_1} < 400");

    auto sensor_5 = make_shared_ext<Sensor>();
    sensor_5->id = "sensor_5";
    sensor_5->metadata = {
            .name = "Sensor 5",
            .unit = "",
            .description = "Test Sensor 5"
    };
    sensor_5->configuration = {
            .type = SensorType::VIRTUAL_INDICATOR,
            .sampling_rate_ms = 1000,
        .expression_evaluator = make_unique_ext<ExpressionEvaluator>(expression_evaluator_5)
    };

    auto sensor_6 = make_shared_ext<Sensor>();
    sensor_6->id = "sensor_6";
    sensor_6->metadata = {
        .name = "Sensor 6",
        .unit = "",
        .description = "Test Sensor 6"
    };
    sensor_6->configuration = {
        .type = SensorType::CANBUS_ANALOG,
        .sampling_rate_ms = 1000,
        .canbus_source = make_unique_ext<CanbusSource>(
            790,
            "RPM"
        )
    };

    std::vector<std::shared_ptr<Sensor>> sensors = {
        sensor_1,
        sensor_2,
        sensor_3,
        // sensor_4,
        // sensor_5,
        sensor_6
    };

    auto res = sensors_configuration_manager->Update(sensors);
    if(!res)
        throw std::runtime_error("Cannot save Sensors config");
}
