#include <zephyr/ztest.h>

#include "utilities/guid/guid_generator.h"
#include "utilities/time/boot_elapsed_time_service.h"
#include "utilities/math_parser/math_parser_service.hpp"
#include "domain/adc_domain/hardware/adc_configuration.h"
#include "domain/adc_domain/hardware/i_adc.h"
#include "domain/adc_domain/hardware/adc_simulator.h"
#include "domain/sensor_domain/utilities/sensors_order_resolver.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensor_reader.h"
#include "domain/sensor_domain/processors/sensor_processor.h"

#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "domain/sensor_domain/utilities/voltage_interpolator/cubic_spline_voltage_interpolator.hpp"

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::domain::adc_domain::hardware;
using namespace eerie_leap::domain::sensor_domain::processors;

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator;

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

    std::vector<std::shared_ptr<Sensor>> sensors = {
        std::make_shared<Sensor>(sensor_1),
        std::make_shared<Sensor>(sensor_2),
        std::make_shared<Sensor>(sensor_3)
    };

    SensorsOrderResolver sensors_order_resolver;
    for(auto& sensor : sensors)
        sensors_order_resolver.AddSensor(sensor);

    return sensors_order_resolver.GetProcessingOrder();
}

struct sensor_processor_HelperInstances {
    std::shared_ptr<MathParserService> math_parser_service;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame;
    std::shared_ptr<SensorReader> sensor_reader;
};

sensor_processor_HelperInstances sensor_processor_GetReadingInstances() {
    auto time_service = std::make_shared<BootElapsedTimeService>();
    time_service->Initialize();
    auto guid_generator = std::make_shared<GuidGenerator>();
    auto math_parser_service = std::make_shared<MathParserService>();

    auto adc = std::make_shared<AdcSimulator>();
    adc->Initialize();
    adc->UpdateConfiguration(AdcConfiguration {
        .samples = 1
    });

    auto sensor_readings_frame = std::make_shared<SensorReadingsFrame>();
    auto sensor_reader = std::make_shared<SensorReader>(time_service, guid_generator, adc, sensor_readings_frame);

    return sensor_processor_HelperInstances {
        .math_parser_service = math_parser_service,
        .sensor_readings_frame = sensor_readings_frame,
        .sensor_reader = sensor_reader
    };
}

ZTEST(sensor_processor, test_ProcessSensorReading) {
        auto helper = sensor_processor_GetReadingInstances();

        auto math_parser_service = helper.math_parser_service;
        auto sensor_readings_frame = helper.sensor_readings_frame;
        auto sensor_reader = helper.sensor_reader;

        auto sensors = sensor_processor_GetTestSensors(math_parser_service);
        for(auto& sensor : sensors)
            sensor_reader->Read(sensor);

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

        auto sensor_1_voltage_interpolator = sensors[1]->configuration.voltage_interpolator;
        double reading_1_value = sensor_1_voltage_interpolator->Interpolate(reading_1->value.value());
        auto sensor_2_voltage_interpolator = sensors[0]->configuration.voltage_interpolator;
        double reading_2_value = sensor_2_voltage_interpolator->Interpolate(reading_2->value.value());

        SensorProcessor sensor_processor(sensor_readings_frame);
        for(auto& sensor : sensors)
            sensor_processor.ProcessSensorReading(sensor_readings_frame->GetReading(sensor->id));

        auto proc_reading_2 = sensor_readings_frame->GetReadings().at("sensor_2");
        zassert_equal(proc_reading_2->status, ReadingStatus::PROCESSED);
        zassert_true(proc_reading_2->value.has_value());
        double proc_reading_2_value = reading_2_value * 4 + 1.6;
        zassert_equal(proc_reading_2->value.value(), proc_reading_2_value);

        auto proc_reading_1 = sensor_readings_frame->GetReadings().at("sensor_1");
        zassert_equal(proc_reading_1->status, ReadingStatus::PROCESSED);
        zassert_true(proc_reading_1->value.has_value());
        double proc_reading_1_value = reading_1_value * 2 + proc_reading_2_value + 1;
        zassert_equal(proc_reading_1->value.value(), proc_reading_1_value);

        auto proc_reading_3 = sensor_readings_frame->GetReadings().at("sensor_3");
        zassert_equal(proc_reading_3->status, ReadingStatus::PROCESSED);
        zassert_true(proc_reading_3->value.has_value());
        double proc_reading_3_value = proc_reading_1_value + 8.34;
        zassert_equal(proc_reading_3->value.value(), proc_reading_3_value);
}
