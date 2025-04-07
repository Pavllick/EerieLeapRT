#pragma once

#include <memory>
#include <zephyr/kernel.h>

#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/processors/sensors_reader.h"
#include "domain/sensor_domain/processors/sensor_processor.h"
#include "sensors_configuration_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::sensor_domain::processors;

class MeasurementService {
private:
    static constexpr int kStackSize = 500;
    static constexpr int kPriority = K_PRIO_PREEMPT(8);
    static void ThreadTrampoline(void* instance, void* p2, void* p3);

    K_KERNEL_STACK_MEMBER(stack_area, kStackSize);

    k_tid_t thread_id_;
    k_thread thread_data_;

    std::shared_ptr<SensorsConfigurationService> sensors_configuration_service_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::shared_ptr<SensorsReader> sensors_reader_;
    std::shared_ptr<SensorProcessor> sensor_processor_;
    
    void EntryPoint();
    void ProcessSensorsReading();

public:
    k_tid_t Start();
};

} // namespace eerie_leap::domain::sensor_domain::services