#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "utilities/time/i_time_service.h"
#include "utilities/guid/guid_generator.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensors_reader.h"
#include "domain/sensor_domain/processors/sensor_processor.h"
#include "sensors_configuration_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::utilities::guid;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::processors;

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
/// Instantiating this class on the heap or regular stack is unsafe and will cause a crash.
///
class MeasurementService {
private:
    const int32_t READING_INTERVAL_MS_ = 1000; 
    static constexpr int kStackSize = 8192;
    static constexpr int kPriority = K_PRIO_PREEMPT(8);
    static void ThreadTrampoline(void* instance, void* p2, void* p3);

    K_KERNEL_STACK_MEMBER(stack_area_, kStackSize);

    k_tid_t thread_id_;
    k_thread thread_data_;
    k_mutex sensors_reading_mutex_;

    std::shared_ptr<ITimeService> time_service_;
    std::shared_ptr<GuidGenerator> guid_generator_;
    std::shared_ptr<Adc> adc_;
    std::shared_ptr<SensorsConfigurationService> sensors_configuration_service_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<SensorsReader> sensors_reader_;
    std::shared_ptr<SensorProcessor> sensor_processor_;
    
    void EntryPoint();
    void ProcessSensorsReading();

public:
    MeasurementService(std::shared_ptr<ITimeService> time_service, std::shared_ptr<GuidGenerator> guid_generator, std::shared_ptr<SensorsConfigurationService> sensors_configuration_service)
        : time_service_(std::move(time_service)), guid_generator_(std::move(guid_generator)), sensors_configuration_service_(std::move(sensors_configuration_service)) { }

    k_tid_t Start();
};

} // namespace eerie_leap::domain::sensor_domain::services