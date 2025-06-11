#include <vector>
#include <zephyr/logging/log.h>

#include "utilities/memory/heap_allocator.hpp"
#include "measurement_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::memory;

LOG_MODULE_REGISTER(measurement_service_logger);

#ifdef CONFIG_SHARED_MULTI_HEAP
Z_KERNEL_STACK_DEFINE_IN(MeasurementService::stack_area_, MeasurementService::k_stack_size_, __attribute__((section(".ext_ram.bss"))));
#else
K_KERNEL_STACK_MEMBER(MeasurementService::stack_area_, MeasurementService::k_stack_size_);
#endif

k_tid_t MeasurementService::Start() {
    thread_id_ = k_thread_create(
        &thread_data_,
        stack_area_,
        K_THREAD_STACK_SIZEOF(stack_area_),
        [](void* instance, void* p2, void* p3) { static_cast<MeasurementService*>(instance)->EntryPoint(); },
        this, nullptr, nullptr,
        k_priority_, 0, K_NO_WAIT);

    return thread_id_;
}

void MeasurementService::EntryPoint() {
    LOG_INF("Measurement Service started");

    processing_scheduler_service_ = make_shared_ext<ProcessingSchedulerService>(time_service_, guid_generator_, adc_, sensors_configuration_controller_);
    processing_scheduler_service_->Start();
}

} // namespace eerie_leap::domain::sensor_domain::services
