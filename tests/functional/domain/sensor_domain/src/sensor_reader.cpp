#include <zephyr/ztest.h>

#include "utilities/guid/guid_generator.h"
#include "utilities/time/boot_elapsed_time_service.h"
#include "utilities/math_parser/math_parser_service.hpp"

#include "configuration/adc_config/adc_config.h"
#include "configuration/services/configuration_service.h"
#include "controllers/adc_configuration_controller.h"
#include "domain/fs_domain/services/fs_service.h"

#include "domain/hardware/adc_domain/models/adc_configuration.h"
#include "domain/hardware/adc_domain/i_adc.h"
#include "domain/hardware/adc_domain/adc_simulator.h"
#include "domain/hardware/gpio_domain/i_gpio.h"
#include "domain/hardware/gpio_domain/gpio_simulator.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensor_reader/i_sensor_reader.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_physical_analog.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_physical_indicator.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_virtual_analog.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_virtual_indicator.h"

#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::domain::hardware::adc_domain;
using namespace eerie_leap::domain::hardware::adc_domain::models;
using namespace eerie_leap::domain::hardware::gpio_domain;
using namespace eerie_leap::domain::sensor_domain::processors;
using namespace eerie_leap::domain::sensor_domain::processors::sensor_reader;

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::utilities::voltage_interpolator;

ZTEST_SUITE(sensors_reader, NULL, NULL, NULL, NULL, NULL);

std::vector<std::shared_ptr<Sensor>> sensors_reader_GetTestSensors(std::shared_ptr<MathParserService> math_parser_service) {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {3.3, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);

    ExpressionEvaluator expression_evaluator_1(math_parser_service, "{x} * 2 + {sensor_2} + 1");

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
            .channel = 1,
            .sampling_rate_ms = 500,
            .voltage_interpolator = std::make_shared<CubicSplineVoltageInterpolator>(calibration_data_2_ptr),
            .expression_evaluator = std::make_shared<ExpressionEvaluator>(expression_evaluator_2)
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
            .expression_evaluator = std::make_shared<ExpressionEvaluator>(expression_evaluator_3)
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
            .expression_evaluator = std::make_shared<ExpressionEvaluator>(expression_evaluator_5)
        }
    };

    std::vector<std::shared_ptr<Sensor>> sensors = {
        std::make_shared<Sensor>(sensor_1),
        std::make_shared<Sensor>(sensor_2),
        std::make_shared<Sensor>(sensor_3),
        std::make_shared<Sensor>(sensor_4),
        std::make_shared<Sensor>(sensor_5)
    };

    return sensors;
}

std::shared_ptr<AdcConfiguration> sensors_reader_GetTestConfiguration() {
    std::vector<CalibrationData> adc_calibration_data_samples {
        {0.0, 0.0},
        {5.0, 5.0}
    };

    auto adc_calibration_data_samples_ptr = make_shared_ext<std::vector<CalibrationData>>(adc_calibration_data_samples);
    auto adc_calibrator = make_shared_ext<AdcCalibrator>(InterpolationMethod::LINEAR, adc_calibration_data_samples_ptr);

    auto adc_channel_configuration = make_shared_ext<AdcChannelConfiguration>(adc_calibrator);

    std::vector<std::shared_ptr<AdcChannelConfiguration>> channel_configurations;
    channel_configurations.reserve(8);
    for(int i = 0; i < 8; i++)
        channel_configurations.push_back(adc_channel_configuration);

    auto adc_configuration = make_shared_ext<AdcConfiguration>();
    adc_configuration->samples = 40;
    adc_configuration->channel_configurations =
        make_shared_ext<std::vector<std::shared_ptr<AdcChannelConfiguration>>>(channel_configurations);

    return adc_configuration;
}

struct sensors_reader_HelperInstances {
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame;
    std::shared_ptr<std::vector<std::shared_ptr<ISensorReader>>> sensor_readers;
};

sensors_reader_HelperInstances sensors_reader_GetReadingInstances() {
    auto fs_service = std::make_shared<FsService>();
    fs_service->Format();

    std::shared_ptr<ITimeService> time_service = std::make_shared<BootElapsedTimeService>();
    time_service->Initialize();
    std::shared_ptr<GuidGenerator> guid_generator = std::make_shared<GuidGenerator>();
    std::shared_ptr<MathParserService> math_parser_service = std::make_shared<MathParserService>();

    const auto adc_configuration = sensors_reader_GetTestConfiguration();

    auto adc_configuration_service = std::make_shared<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_configuration_service);
    adc_configuration_controller->Update(adc_configuration);

    auto gpio = std::make_shared<GpioSimulator>();
    gpio->Initialize();

    auto sensor_readings_frame = std::make_shared<SensorReadingsFrame>();
    auto sensors = sensors_reader_GetTestSensors(math_parser_service);

    auto sensor_readers = std::make_shared<std::vector<std::shared_ptr<ISensorReader>>>();
    for(int i = 0; i < sensors.size(); i++) {
        std::shared_ptr<ISensorReader> sensor_reader;

        if(sensors[i]->configuration.type == SensorType::PHYSICAL_ANALOG) {
            sensor_reader = make_shared_ext<SensorReaderPhysicalAnalog>(
                time_service,
                guid_generator,
                sensor_readings_frame,
                sensors[i],
                adc_configuration_controller);
        } else if(sensors[i]->configuration.type == SensorType::VIRTUAL_ANALOG) {
            sensor_reader = make_shared_ext<SensorReaderVirtualAnalog>(
                time_service,
                guid_generator,
                sensor_readings_frame,
                sensors[i]);
        } else if(sensors[i]->configuration.type == SensorType::PHYSICAL_INDICATOR) {
            sensor_reader = make_shared_ext<SensorReaderPhysicalIndicator>(
                time_service,
                guid_generator,
                sensor_readings_frame,
                sensors[i],
                gpio);
        } else if(sensors[i]->configuration.type == SensorType::VIRTUAL_INDICATOR) {
            sensor_reader = make_shared_ext<SensorReaderVirtualIndicator>(
                time_service,
                guid_generator,
                sensor_readings_frame,
                sensors[i]);
        } else {
            throw std::runtime_error("Unsupported sensor type");
        }

        sensor_readers->push_back(sensor_reader);
    }

    return sensors_reader_HelperInstances {
        .sensor_readings_frame = sensor_readings_frame,
        .sensor_readers = sensor_readers
    };
}

ZTEST(sensors_reader, test_Read) {
    auto helper = sensors_reader_GetReadingInstances();

    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensor_readers = helper.sensor_readers;

    auto readings = sensor_readings_frame->GetReadings();
    zassert_equal(readings.size(), 0);

    for(int i = 0; i < sensor_readers->size(); i++)
        sensor_readers->at(i)->Read();

    readings = sensor_readings_frame->GetReadings();
    zassert_equal(readings.size(), 5);

    auto reading = sensor_readings_frame->GetReadings().at("sensor_2");
    zassert_equal(reading->status, ReadingStatus::RAW);
    zassert_true(reading->value.has_value());
    zassert_true(reading->timestamp.has_value());
    zassert_true(reading->id.AsUint64() > 0);
    zassert_between_inclusive(reading->value.value(), 0, 3.3);

    reading = sensor_readings_frame->GetReadings().at("sensor_1");
    zassert_equal(reading->status, ReadingStatus::RAW);
    zassert_true(reading->value.has_value());
    zassert_true(reading->timestamp.has_value());
    zassert_true(reading->id.AsUint64() > 0);
    zassert_between_inclusive(reading->value.value(), 0, 3.3);

    reading = sensor_readings_frame->GetReadings().at("sensor_3");
    zassert_equal(reading->status, ReadingStatus::UNINITIALIZED);
    zassert_true(reading->timestamp.has_value());
    zassert_true(reading->id.AsUint64() > 0);
    zassert_false(reading->value.has_value());

    reading = sensor_readings_frame->GetReadings().at("sensor_4");
    zassert_equal(reading->status, ReadingStatus::RAW);
    zassert_true(reading->timestamp.has_value());
    zassert_true(reading->id.AsUint64() > 0);
    zassert_true(reading->value.has_value());
    zassert_true(reading->value.value() == 1 || reading->value.value() == 0);

    reading = sensor_readings_frame->GetReadings().at("sensor_5");
    zassert_equal(reading->status, ReadingStatus::UNINITIALIZED);
    zassert_true(reading->timestamp.has_value());
    zassert_true(reading->id.AsUint64() > 0);
    zassert_false(reading->value.has_value());
}
