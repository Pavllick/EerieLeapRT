#include <zephyr/logging/log.h>

#include "measurement_processing_thread.h"
#include "domain/adc_domain/adc.h"

namespace eerie_leap::threads {

using namespace eerie_leap::domain;

LOG_MODULE_REGISTER(measurement_processing_thread_logger);

k_tid_t MeasurementProcessingThread::Start() {
    thread_id = k_thread_create(
        &thread_data,
        stack_area,
        K_THREAD_STACK_SIZEOF(stack_area),
        ThreadTrampoline,
        this, NULL, NULL,
        kPriority, 0, K_NO_WAIT);

    return thread_id;
}

void MeasurementProcessingThread::ThreadTrampoline(void* instance, void* p2, void* p3) {
    static_cast<MeasurementProcessingThread*>(instance)->EntryPoint();
}

void MeasurementProcessingThread::EntryPoint() {
    LOG_INF("Measurement Processing Thread started");
    
    adc_domain::Adc adc;
    adc.Initialize();
}

} // namespace eerie_leap::threads