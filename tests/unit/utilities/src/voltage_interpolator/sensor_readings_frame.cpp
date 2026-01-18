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
    std::pmr::vector<CalibrationData> calibration_data_1 {
        {0.0, 0.0},
        {3.3, 100.0}
    };
    auto calibration_data_1_ptr = std::make_shared<std::pmr::vector<CalibrationData>>(calibration_data_1);

    auto sensor_1 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_1");

    sensor_1->metadata.name = "Sensor 1";
    sensor_1->metadata.unit = "km/h";
    sensor_1->metadata.description = "Test Sensor 1";

    sensor_1->configuration.type = SensorType::PHYSICAL_ANALOG;
    sensor_1->configuration.channel = 0;
    sensor_1->configuration.sampling_rate_ms = 1000;
    sensor_1->configuration.voltage_interpolator = make_unique_pmr<LinearVoltageInterpolator>(Mrm::GetDefaultPmr(), calibration_data_1_ptr);
    sensor_1->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(Mrm::GetDefaultPmr(), "x * 2 + sensor_2 + 1");

    std::pmr::vector<CalibrationData> calibration_data_2 {
        {0.0, 0.0},
        {1.0, 29.0},
        {2.0, 111.0},
        {2.5, 162.0},
        {3.3, 200.0}
    };
    auto calibration_data_2_ptr = std::make_shared<std::pmr::vector<CalibrationData>>(calibration_data_2);

    auto sensor_2 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_2");

    sensor_2->metadata.name = "Sensor 2";
    sensor_2->metadata.unit = "km/h";
    sensor_2->metadata.description = "Test Sensor 2";

    sensor_2->configuration.type = SensorType::PHYSICAL_ANALOG;
    sensor_2->configuration.channel = 1;
    sensor_2->configuration.sampling_rate_ms = 500;
    sensor_2->configuration.voltage_interpolator = make_unique_pmr<LinearVoltageInterpolator>(Mrm::GetDefaultPmr(), calibration_data_2_ptr);
    sensor_2->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(Mrm::GetDefaultPmr(), "x * 4 + 1.6");

    auto sensor_3 = std::make_shared<Sensor>(std::allocator_arg, Mrm::GetDefaultPmr(), "sensor_3");

    sensor_3->metadata.name = "Sensor 3";
    sensor_3->metadata.unit = "km/h";
    sensor_3->metadata.description = "Test Sensor 3";

    sensor_3->configuration.type = SensorType::VIRTUAL_ANALOG;
    sensor_3->configuration.channel = std::nullopt;
    sensor_3->configuration.sampling_rate_ms = 2000;
    sensor_3->configuration.expression_evaluator = make_unique_pmr<ExpressionEvaluator>(Mrm::GetDefaultPmr(), "sensor_1 + 8.34");

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
    auto sensor_readings_frame = make_shared_pmr<SensorReadingsFrame>(Mrm::GetDefaultPmr());

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

        SensorReading reading1(std::allocator_arg, Mrm::GetDefaultPmr(), guid_generator->Generate(), sensors[1]);
        reading1.source = ReadingSource::PROCESSING;
        sensor_readings_frame->AddOrUpdateReading(reading1);

        zassert_equal(sensor_readings_frame->HasReading("sensor_2"), true);

        auto fr_reading_1 = sensor_readings_frame->GetReading("sensor_2");
        zassert_equal(fr_reading_1.status, ReadingStatus::UNINITIALIZED);
        zassert_false(fr_reading_1.value.has_value());

        SensorReading reading2(std::allocator_arg, Mrm::GetDefaultPmr(), guid_generator->Generate(), sensors[1]);
        reading2.source = ReadingSource::PROCESSING;
        reading2.status = ReadingStatus::PROCESSED;
        reading2.value = 1.6;
        sensor_readings_frame->AddOrUpdateReading(reading2);

        zassert_equal(sensor_readings_frame->HasReading("sensor_2"), true);

        auto fr_reading_2 = sensor_readings_frame->GetReading("sensor_2");
        zassert_equal(fr_reading_2.status, ReadingStatus::PROCESSED);
        zassert_true(fr_reading_2.value.has_value());

        SensorReading reading3(std::allocator_arg, Mrm::GetDefaultPmr(), guid_generator->Generate(), sensors[2]);
        reading3.source = ReadingSource::PROCESSING;
        sensor_readings_frame->AddOrUpdateReading(reading3);

        zassert_equal(sensor_readings_frame->HasReading("sensor_3"), true);
}

ZTEST(sensor_readings_frame, test_GetReading) {
    auto helper = sensor_readings_frame_GetHelperInstances();

    auto guid_generator = helper.guid_generator;
    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensors = sensor_readings_frame_GetTestSensors();

    SensorReading reading1(std::allocator_arg, Mrm::GetDefaultPmr(), guid_generator->Generate(), sensors[1]);
    reading1.source = ReadingSource::PROCESSING;
    sensor_readings_frame->AddOrUpdateReading(reading1);
    SensorReading reading2(std::allocator_arg, Mrm::GetDefaultPmr(), guid_generator->Generate(), sensors[2]);
    reading2.source = ReadingSource::PROCESSING;
    sensor_readings_frame->AddOrUpdateReading(reading2);

    auto rf_reading1 = sensor_readings_frame->GetReading("sensor_2");
    zassert_str_equal(rf_reading1.sensor->id.c_str(), "sensor_2");

    auto rf_reading2 = sensor_readings_frame->GetReading("sensor_3");
    zassert_str_equal(rf_reading2.sensor->id.c_str(), "sensor_3");
}

ZTEST_EXPECT_FAIL(sensor_readings_frame, test_GetReading_no_sensor);
ZTEST(sensor_readings_frame, test_GetReading_no_sensor) {
    auto helper = sensor_readings_frame_GetHelperInstances();

    auto guid_generator = helper.guid_generator;
    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensors = sensor_readings_frame_GetTestSensors();

    try {
        sensor_readings_frame->GetReading("sensor_2");
        zassert_true(true, "GetReading expected to fail, but it didn't.");
    } catch(...) {
        zassert_true(false, "GetReading failed as expected due to missing sensor reading.");
    }
}

ZTEST(sensor_readings_frame, test_ClearReadings) {
    auto helper = sensor_readings_frame_GetHelperInstances();

    auto guid_generator = helper.guid_generator;
    auto sensor_readings_frame = helper.sensor_readings_frame;
    auto sensors = sensor_readings_frame_GetTestSensors();

    auto readings = sensor_readings_frame->GetProcessedReadings();
    zassert_equal(readings.size(), 0);

    SensorReading reading1(std::allocator_arg, Mrm::GetDefaultPmr(), guid_generator->Generate(), sensors[0]);
    reading1.source = ReadingSource::ISR;
    reading1.value = 2.4;
    reading1.status = ReadingStatus::RAW;
    sensor_readings_frame->AddOrUpdateReading(reading1);

    SensorReading reading2(std::allocator_arg, Mrm::GetDefaultPmr(), guid_generator->Generate(), sensors[1]);
    reading2.source = ReadingSource::PROCESSING;
    reading2.status = ReadingStatus::ERROR;
    sensor_readings_frame->AddOrUpdateReading(reading2);

    SensorReading reading3(std::allocator_arg, Mrm::GetDefaultPmr(), guid_generator->Generate(), sensors[2]);
    reading3.source = ReadingSource::PROCESSING;
    reading3.value = 2.6;
    reading3.status = ReadingStatus::PROCESSED;
    sensor_readings_frame->AddOrUpdateReading(reading3);

    zassert_equal(sensor_readings_frame->HasIsrReading("sensor_1"), true);
    zassert_equal(sensor_readings_frame->HasIsrReading("sensor_2"), false);
    zassert_equal(sensor_readings_frame->HasIsrReading("sensor_3"), false);

    zassert_equal(sensor_readings_frame->HasReading("sensor_1"), false);
    zassert_equal(sensor_readings_frame->HasReading("sensor_2"), true);
    zassert_equal(sensor_readings_frame->HasReading("sensor_3"), true);

    zassert_equal(sensor_readings_frame->HasProcessedReading("sensor_1"), false);
    zassert_equal(sensor_readings_frame->HasProcessedReading("sensor_2"), false);
    zassert_equal(sensor_readings_frame->HasProcessedReading("sensor_3"), true);

    zassert_equal(sensor_readings_frame->HasReadingValue("sensor_1"), false);
    zassert_equal(sensor_readings_frame->HasReadingValue("sensor_2"), false);
    zassert_equal(sensor_readings_frame->HasReadingValue("sensor_3"), true);

    sensor_readings_frame->ClearReadings();

    zassert_equal(sensor_readings_frame->HasIsrReading("sensor_1"), false);
    zassert_equal(sensor_readings_frame->HasIsrReading("sensor_2"), false);
    zassert_equal(sensor_readings_frame->HasIsrReading("sensor_3"), false);

    zassert_equal(sensor_readings_frame->HasReading("sensor_1"), false);
    zassert_equal(sensor_readings_frame->HasReading("sensor_2"), false);
    zassert_equal(sensor_readings_frame->HasReading("sensor_3"), false);

    zassert_equal(sensor_readings_frame->HasProcessedReading("sensor_1"), false);
    zassert_equal(sensor_readings_frame->HasProcessedReading("sensor_2"), false);
    zassert_equal(sensor_readings_frame->HasProcessedReading("sensor_3"), false);

    zassert_equal(sensor_readings_frame->HasReadingValue("sensor_1"), false);
    zassert_equal(sensor_readings_frame->HasReadingValue("sensor_2"), false);
    zassert_equal(sensor_readings_frame->HasReadingValue("sensor_3"), false);
}
