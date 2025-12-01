#include <zephyr/ztest.h>

#include "utilities/guid/guid_generator.h"
#include "utilities/string/string_helpers.h"
#include "utilities/voltage_interpolator/linear_voltage_interpolator.hpp"
#include "subsys/math_parser/expression_evaluator.h"
#include "domain/sensor_domain/models/sensor.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"

using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::utilities::string;
using namespace eerie_leap::utilities::voltage_interpolator;
using namespace eerie_leap::subsys::math_parser;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;

ZTEST_SUITE(sensor_readings_frame, NULL, NULL, NULL, NULL, NULL);

std::vector<std::shared_ptr<Sensor>> sensor_readings_frame_GetTestSensors() {
    std::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {3.3, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_1);

    auto sensor_1 = std::make_shared<Sensor>("sensor_1");
    sensor_1->metadata = {
        .name = "Sensor 1",
        .unit = "km/h",
        .description = "Test Sensor 1"
    };
    sensor_1->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 0,
        .sampling_rate_ms = 1000,
        .voltage_interpolator = std::make_unique<LinearVoltageInterpolator>(calibration_data_1_ptr),
        .expression_evaluator = std::make_unique<ExpressionEvaluator>("x * 2 + sensor_2 + 1")
    };

    std::vector<CalibrationData> calibration_data_2 {
        {0.0, 0.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = std::make_shared<std::vector<CalibrationData>>(calibration_data_2);

    auto sensor_2 = std::make_shared<Sensor>("sensor_2");
    sensor_2->metadata = {
        .name = "Sensor 2",
        .unit = "km/h",
        .description = "Test Sensor 2"
    };
    sensor_2->configuration = {
        .type = SensorType::PHYSICAL_ANALOG,
        .channel = 1,
        .sampling_rate_ms = 500,
        .voltage_interpolator = std::make_unique<LinearVoltageInterpolator>(calibration_data_2_ptr),
        .expression_evaluator = std::make_unique<ExpressionEvaluator>("x * 4 + 1.6")
    };

    auto sensor_3 = std::make_shared<Sensor>("sensor_3");
    sensor_3->metadata = {
        .name = "Sensor 3",
        .unit = "km/h",
        .description = "Test Sensor 3"
    };
    sensor_3->configuration = {
        .type = SensorType::VIRTUAL_ANALOG,
        .sampling_rate_ms = 2000,
        .expression_evaluator = std::make_unique<ExpressionEvaluator>("sensor_1 + 8.34")
    };

    std::vector<std::shared_ptr<Sensor>> sensors = {
        sensor_1, sensor_2, sensor_3 };

    return sensors;
}

struct sensor_readings_frame_GetTestSensors_HelperInstances {
    std::shared_ptr<GuidGenerator> guid_generator;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame;
};

sensor_readings_frame_GetTestSensors_HelperInstances sensor_readings_frame_GetHelperInstances() {
    auto guid_generator = std::make_shared<GuidGenerator>();
    auto sensor_readings_frame = std::make_shared<SensorReadingsFrame>();

    return sensor_readings_frame_GetTestSensors_HelperInstances {
        .guid_generator = guid_generator,
        .sensor_readings_frame = sensor_readings_frame
    };
}

ZTEST(sensor_readings_frame, test_AddOrUpdateReading) {
        auto helper = sensor_readings_frame_GetHelperInstances();

        auto guid_generator = helper.guid_generator;
        auto sensor_readings_frame = helper.sensor_readings_frame;
        auto sensors = sensor_readings_frame_GetTestSensors();

        auto readings = sensor_readings_frame->GetReadings();
        zassert_equal(readings.size(), 0);

        auto reading1 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[1]);
        sensor_readings_frame->AddOrUpdateReading(reading1);

        readings = sensor_readings_frame->GetReadings();
        zassert_equal(readings.size(), 1);

        auto fr_reading = sensor_readings_frame->GetReadings().at(StringHelpers::GetHash("sensor_2"));
        zassert_equal(fr_reading->status, ReadingStatus::UNINITIALIZED);
        zassert_false(fr_reading->value.has_value());

        auto reading2 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[1]);
        reading2->status = ReadingStatus::PROCESSED;
        reading2->value = 1.6;
        sensor_readings_frame->AddOrUpdateReading(reading2);

        readings = sensor_readings_frame->GetReadings();
        zassert_equal(readings.size(), 1);

        fr_reading = sensor_readings_frame->GetReadings().at(StringHelpers::GetHash("sensor_2"));
        zassert_equal(fr_reading->status, ReadingStatus::PROCESSED);
        zassert_true(fr_reading->value.has_value());

        auto reading3 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[2]);
        sensor_readings_frame->AddOrUpdateReading(reading3);

        readings = sensor_readings_frame->GetReadings();
        zassert_equal(readings.size(), 2);
}

ZTEST(sensor_readings_frame, test_GetReading) {
    auto helper = sensor_readings_frame_GetHelperInstances();

    auto guid_generator = helper.guid_generator;
    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensors = sensor_readings_frame_GetTestSensors();

    auto reading1 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[1]);
    sensor_readings_frame->AddOrUpdateReading(reading1);
    auto reading2 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[2]);
    sensor_readings_frame->AddOrUpdateReading(reading2);

    auto rf_reading1 = sensor_readings_frame->GetReading(StringHelpers::GetHash("sensor_2"));
    zassert_str_equal(rf_reading1->sensor->id.c_str(), "sensor_2");

    auto rf_reading2 = sensor_readings_frame->GetReading(StringHelpers::GetHash("sensor_3"));
    zassert_str_equal(rf_reading2->sensor->id.c_str(), "sensor_3");
}

ZTEST_EXPECT_FAIL(sensor_readings_frame, test_GetReading_no_sensor);
ZTEST(sensor_readings_frame, test_GetReading_no_sensor) {
    auto helper = sensor_readings_frame_GetHelperInstances();

    auto guid_generator = helper.guid_generator;
    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensors = sensor_readings_frame_GetTestSensors();

    try {
        sensor_readings_frame->GetReading(StringHelpers::GetHash("sensor_2"));
        zassert_true(true, "GetReading expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "GetReading failed as expected due to missing sensor reading.");
    }

    auto reading1 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[1]);
    sensor_readings_frame->AddOrUpdateReading(reading1);
    auto reading2 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[2]);
    sensor_readings_frame->AddOrUpdateReading(reading2);

    auto rf_reading1 = sensor_readings_frame->GetReading(StringHelpers::GetHash("sensor_2"));
    zassert_str_equal(rf_reading1->sensor->id.c_str(), "sensor_2");

    auto rf_reading2 = sensor_readings_frame->GetReading(StringHelpers::GetHash("sensor_3"));
    zassert_str_equal(rf_reading2->sensor->id.c_str(), "sensor_3");

    try {
        sensor_readings_frame->GetReading(StringHelpers::GetHash("sensor_1"));
        zassert_true(true, "GetReading expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "GetReading failed as expected due to missing sensor reading.");
    }
}

ZTEST(sensor_readings_frame, test_GetReadings) {
    auto helper = sensor_readings_frame_GetHelperInstances();

    auto guid_generator = helper.guid_generator;
    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensors = sensor_readings_frame_GetTestSensors();

    auto readings = sensor_readings_frame->GetReadings();
    zassert_equal(readings.size(), 0);

    auto reading1 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[1]);
    sensor_readings_frame->AddOrUpdateReading(reading1);
    auto reading2 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[2]);
    sensor_readings_frame->AddOrUpdateReading(reading2);

    auto rf_reading1 = sensor_readings_frame->GetReading(StringHelpers::GetHash("sensor_2"));
    zassert_str_equal(rf_reading1->sensor->id.c_str(), "sensor_2");

    auto rf_reading2 = sensor_readings_frame->GetReading(StringHelpers::GetHash("sensor_3"));
    zassert_str_equal(rf_reading2->sensor->id.c_str(), "sensor_3");

    readings = sensor_readings_frame->GetReadings();
    zassert_equal(readings.size(), 2);

    auto fr_reading1 = sensor_readings_frame->GetReadings().at(StringHelpers::GetHash("sensor_2"));
    zassert_str_equal(fr_reading1->sensor->id.c_str(), "sensor_2");
    auto fr_reading2 = sensor_readings_frame->GetReadings().at(StringHelpers::GetHash("sensor_3"));
    zassert_str_equal(fr_reading2->sensor->id.c_str(), "sensor_3");
}

ZTEST(sensor_readings_frame, test_ClearReadings) {
    auto helper = sensor_readings_frame_GetHelperInstances();

    auto guid_generator = helper.guid_generator;
    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensors = sensor_readings_frame_GetTestSensors();

    auto readings = sensor_readings_frame->GetReadings();
    zassert_equal(readings.size(), 0);
    auto reading_values = sensor_readings_frame->GetReadings();
    zassert_equal(reading_values.size(), 0);

    auto reading1 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[0]);
    reading1->value = 2.4;
    reading1->status = ReadingStatus::PROCESSED;
    sensor_readings_frame->AddOrUpdateReading(reading1);
    auto reading2 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[1]);
    reading2->status = ReadingStatus::ERROR;
    sensor_readings_frame->AddOrUpdateReading(reading2);
    auto reading3 = std::make_shared<SensorReading>(guid_generator->Generate(), sensors[2]);
    reading3->value = 2.6;
    reading3->status = ReadingStatus::PROCESSED;
    sensor_readings_frame->AddOrUpdateReading(reading3);

    readings = sensor_readings_frame->GetReadings();
    zassert_equal(readings.size(), 3);

    zassert_equal(sensor_readings_frame->HasReadingValue(StringHelpers::GetHash("sensor_1")), true);
    zassert_equal(sensor_readings_frame->HasReadingValue(StringHelpers::GetHash("sensor_2")), false);
    zassert_equal(sensor_readings_frame->HasReadingValue(StringHelpers::GetHash("sensor_3")), true);

    sensor_readings_frame->ClearReadings();

    readings = sensor_readings_frame->GetReadings();
    zassert_equal(readings.size(), 0);
    reading_values = sensor_readings_frame->GetReadings();
    zassert_equal(reading_values.size(), 0);
}
