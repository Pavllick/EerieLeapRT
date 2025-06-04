#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "utilities/math_parser/math_parser_service.hpp"
#include "controllers/sensors_configuration_controller.h"
#include "domain/adc_domain/hardware/i_adc.h"
#include "domain/sensor_domain/services/scheduler/processing_scheduler_serivce.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::utilities::math_parser;
using namespace eerie_leap::controllers;
using namespace eerie_leap::domain::sensor_domain::services::scheduler;


/// Service responsible for reading and processing sensor data in a dedicated thread.
///
/// NOTE:
/// This class contains a Zephyr thread stack declared with `K_KERNEL_STACK_MEMBER`.
/// As a result, it must be constructed in statically allocated, properly aligned memory,
/// such as via `placement-new` on a `static alignas(4) uint8_t` buffer.
///
/// Example usage:
/// ```cpp
/// alignas(4) static uint8_t buffer[sizeof(MeasurementService)];
/// auto* service = new (buffer) MeasurementService();
/// service->Start();
/// ```
///
class MeasurementService {
private:
    const int32_t READING_INTERVAL_MS_ = 1000;
    static constexpr int k_stack_size_ = 32768;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(2);

    K_KERNEL_STACK_MEMBER(stack_area_, k_stack_size_);

    k_tid_t thread_id_;
    k_thread thread_data_;

    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;

    std::shared_ptr<IAdc> adc_;
    std::shared_ptr<MathParserService> math_parser_service_;
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller_;
    std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service_;

    void EntryPoint();
    void SetupTestSensors();

public:
    MeasurementService(
        std::shared_ptr<ITimeService> time_service,
        std::shared_ptr<GuidGenerator> guid_generator,
        std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller)
        : time_service_(std::move(time_service)),
        guid_generator_(std::move(guid_generator)),
        sensors_configuration_controller_(std::move(sensors_configuration_controller)) { }

    k_tid_t Start();
};

} // namespace eerie_leap::domain::sensor_domain::services
