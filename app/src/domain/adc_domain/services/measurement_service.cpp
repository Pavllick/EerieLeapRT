#include <zephyr/logging/log.h>

#include "measurement_service.h"
#include "domain/adc_domain/hardware/adc.h"

namespace eerie_leap::domain::adc_domain::services {

using namespace eerie_leap::domain;

LOG_MODULE_REGISTER(measurement_service_logger);

k_tid_t MeasurementService::Start() {
    thread_id = k_thread_create(
        &thread_data,
        stack_area,
        K_THREAD_STACK_SIZEOF(stack_area),
        ThreadTrampoline,
        this, NULL, NULL,
        kPriority, 0, K_NO_WAIT);

    return thread_id;
}

void MeasurementService::ThreadTrampoline(void* instance, void* p2, void* p3) {
    static_cast<MeasurementService*>(instance)->EntryPoint();
}

void MeasurementService::EntryPoint() {
    LOG_INF("Measurement Service started");
    
    adc_domain::hardware::Adc adc;
    adc.Initialize();
}

} // namespace eerie_leap::domain::adc_domain::services