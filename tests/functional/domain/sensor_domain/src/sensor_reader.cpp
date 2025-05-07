#include <zephyr/ztest.h>

#include "utilities/guid/guid_generator.h"
#include "utilities/time/boot_elapsed_time_service.h"
#include "utilities/math_parser/math_parser_service.hpp"
#include "domain/adc_domain/hardware/adc_configuration.h"
#include "domain/adc_domain/hardware/i_adc.h"
#include "domain/adc_domain/hardware/adc_simulator.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensor_reader.h"

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
using namespace eerie_leap::domain::sensor_domain::utilities::voltage_interpolator;

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

    std::vector<std::shared_ptr<Sensor>> sensors = {
        std::make_shared<Sensor>(sensor_1),
        std::make_shared<Sensor>(sensor_2),
        std::make_shared<Sensor>(sensor_3)
    };

    return sensors;
}

struct sensors_reader_HelperInstances {
    std::shared_ptr<MathParserService> math_parser_service;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame;
    std::shared_ptr<SensorReader> sensor_reader;
};

sensors_reader_HelperInstances sensors_reader_GetReadingInstances() {
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

    return sensors_reader_HelperInstances {
        .math_parser_service = math_parser_service,
        .sensor_readings_frame = sensor_readings_frame,
        .sensor_reader = sensor_reader
    };
}

ZTEST(sensors_reader, test_Read) {
        auto helper = sensors_reader_GetReadingInstances();

        auto math_parser_service = helper.math_parser_service;
        auto sensor_readings_frame = helper.sensor_readings_frame;
        auto sensor_reader = helper.sensor_reader;

        auto readings = sensor_readings_frame->GetReadings();
        zassert_equal(readings.size(), 0);

        auto sensors = sensors_reader_GetTestSensors(math_parser_service);
        for(auto& sensor : sensors)
            sensor_reader->Read(sensor);

        readings = sensor_readings_frame->GetReadings();
        zassert_equal(readings.size(), 3);

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
}
