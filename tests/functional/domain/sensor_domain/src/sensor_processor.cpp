#include <zephyr/ztest.h>

#include "utilities/guid/guid_generator.h"
#include "utilities/time/boot_elapsed_time_service.h"
#include "utilities/math_parser/math_parser_service.hpp"

#include "domain/hardware/adc_domain/models/adc_configuration.h"
#include "domain/hardware/adc_domain/i_adc.h"
#include "domain/hardware/adc_domain/adc_simulator.h"
#include "domain/hardware/gpio_domain/i_gpio.h"
#include "domain/hardware/gpio_domain/gpio_simulator.h"
#include "domain/fs_domain/services/fs_service.h"

#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensor_reader/i_sensor_reader.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_physical_analog.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_physical_indicator.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_virtual_analog.h"
#include "domain/sensor_domain/processors/sensor_reader/sensor_reader_virtual_indicator.h"
#include "domain/sensor_domain/processors/sensor_processor.h"

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
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities::voltage_interpolator;

ZTEST_SUITE(sensor_processor, NULL, NULL, NULL, NULL, NULL);

std::vector<std::shared_ptr<Sensor>> sensor_processor_GetTestSensors(std::shared_ptr<MathParserService> math_parser_service) {
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

    SensorsOrderResolver sensors_order_resolver;
    for(auto& sensor : sensors)
        sensors_order_resolver.AddSensor(sensor);

    return sensors_order_resolver.GetProcessingOrder();
}

std::shared_ptr<AdcConfiguration> sensor_processor_GetTestConfiguration() {
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

struct sensor_processor_HelperInstances {
    std::shared_ptr<MathParserService> math_parser_service;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame;
    std::shared_ptr<std::vector<std::shared_ptr<ISensorReader>>> sensor_readers;
};

sensor_processor_HelperInstances sensor_processor_GetReadingInstances() {
    auto fs_service = std::make_shared<FsService>();
    fs_service->Format();

    std::shared_ptr<ITimeService> time_service = std::make_shared<BootElapsedTimeService>();
    time_service->Initialize();
    std::shared_ptr<GuidGenerator> guid_generator = std::make_shared<GuidGenerator>();
    std::shared_ptr<MathParserService> math_parser_service = std::make_shared<MathParserService>();

    const auto adc_configuration = sensor_processor_GetTestConfiguration();

    auto adc_configuration_service = std::make_shared<ConfigurationService<AdcConfig>>("adc_config", fs_service);
    auto adc_configuration_controller = std::make_shared<AdcConfigurationController>(adc_configuration_service);
    adc_configuration_controller->Update(adc_configuration);

    auto gpio = std::make_shared<GpioSimulator>();
    gpio->Initialize();

    auto sensor_readings_frame = std::make_shared<SensorReadingsFrame>();
    auto sensors = sensor_processor_GetTestSensors(math_parser_service);

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

    return sensor_processor_HelperInstances {
        .math_parser_service = math_parser_service,
        .sensor_readings_frame = sensor_readings_frame,
        .sensor_readers = sensor_readers
    };
}

ZTEST(sensor_processor, test_ProcessReading) {
    auto helper = sensor_processor_GetReadingInstances();

    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensor_readers = helper.sensor_readers;

    for(int i = 0; i < sensor_readers->size(); i++)
        sensor_readers->at(i)->Read();

    auto reading_2 = sensor_readings_frame->GetReadings().at("sensor_2");
    zassert_equal(reading_2->status, ReadingStatus::RAW);
    zassert_true(reading_2->value.has_value());
    zassert_between_inclusive(reading_2->value.value(), 0, 3.3);

    auto reading_1 = sensor_readings_frame->GetReadings().at("sensor_1");
    zassert_equal(reading_1->status, ReadingStatus::RAW);
    zassert_true(reading_1->value.has_value());
    zassert_between_inclusive(reading_1->value.value(), 0, 3.3);

    auto reading_3 = sensor_readings_frame->GetReadings().at("sensor_3");
    zassert_equal(reading_3->status, ReadingStatus::UNINITIALIZED);
    zassert_false(reading_3->value.has_value());

    auto reading_4 = sensor_readings_frame->GetReadings().at("sensor_4");
    zassert_equal(reading_4->status, ReadingStatus::RAW);
    zassert_true(reading_4->value.has_value());
    zassert_true(reading_4->value.value() == 1 || reading_4->value.value() == 0);

    auto reading_5 = sensor_readings_frame->GetReadings().at("sensor_5");
    zassert_equal(reading_5->status, ReadingStatus::UNINITIALIZED);
    zassert_false(reading_5->value.has_value());

    auto sensors = sensor_processor_GetTestSensors(helper.math_parser_service);

    auto sensor_1_voltage_interpolator = sensors[1]->configuration.voltage_interpolator;
    float reading_1_value = sensor_1_voltage_interpolator->Interpolate(reading_1->value.value());
    auto sensor_2_voltage_interpolator = sensors[0]->configuration.voltage_interpolator;
    float reading_2_value = sensor_2_voltage_interpolator->Interpolate(reading_2->value.value());

    SensorProcessor sensor_processor(sensor_readings_frame);
    for(auto& sensor : sensors)
        sensor_processor.ProcessReading(sensor_readings_frame->GetReading(sensor->id));

    auto proc_reading_2 = sensor_readings_frame->GetReadings().at("sensor_2");
    zassert_equal(proc_reading_2->status, ReadingStatus::PROCESSED);
    zassert_true(proc_reading_2->value.has_value());
    float proc_reading_2_value = reading_2_value * 4 + 1.6;
    zassert_equal(proc_reading_2->value.value(), proc_reading_2_value);

    auto proc_reading_1 = sensor_readings_frame->GetReadings().at("sensor_1");
    zassert_equal(proc_reading_1->status, ReadingStatus::PROCESSED);
    zassert_true(proc_reading_1->value.has_value());
    float proc_reading_1_value = reading_1_value * 2 + proc_reading_2_value + 1;
    zassert_equal(proc_reading_1->value.value(), proc_reading_1_value);

    auto proc_reading_3 = sensor_readings_frame->GetReadings().at("sensor_3");
    zassert_equal(proc_reading_3->status, ReadingStatus::PROCESSED);
    zassert_true(proc_reading_3->value.has_value());
    float proc_reading_3_value = proc_reading_1_value + 8.34;
    zassert_equal(proc_reading_3->value.value(), proc_reading_3_value);

    auto proc_reading_4 = sensor_readings_frame->GetReadings().at("sensor_4");
    zassert_equal(proc_reading_4->status, ReadingStatus::PROCESSED);
    zassert_true(proc_reading_4->value.has_value());
    zassert_true(proc_reading_4->value.value() == 1 || proc_reading_4->value.value() == 0);

    auto proc_reading_5 = sensor_readings_frame->GetReadings().at("sensor_5");
    zassert_equal(proc_reading_5->status, ReadingStatus::PROCESSED);
    zassert_true(proc_reading_5->value.has_value());
    zassert_true(proc_reading_5->value.value() ==  proc_reading_1_value < 400);
}
