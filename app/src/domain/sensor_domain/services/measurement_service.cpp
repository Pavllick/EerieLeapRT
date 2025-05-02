#include <vector>
#include <zephyr/logging/log.h>

#include "measurement_service.h"
#include "domain/adc_domain/hardware/adc.h"
#include "domain/adc_domain/hardware/adc_emulator.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::domain::adc_domain::hardware;

LOG_MODULE_REGISTER(measurement_service_logger);

k_tid_t MeasurementService::Start() {
    thread_id_ = k_thread_create(
        &thread_data_,
        stack_area_,
        K_THREAD_STACK_SIZEOF(stack_area_),
        [](void* instance, void* p2, void* p3) { static_cast<MeasurementService*>(instance)->EntryPoint(); },
        this, NULL, NULL,
        kPriority, 0, K_NO_WAIT);

    return thread_id_;
}

void MeasurementService::EntryPoint() {
    LOG_INF("Measurement Service started");

#ifdef CONFIG_ADC_EMUL
    adc_ = std::make_shared<AdcEmulator>();
#else
    adc_ = std::make_shared<Adc>();
#endif
    adc_->UpdateConfiguration(AdcConfig{
        .channel_count = 4,
        .resolution = 12,
        .samples = 1,
        .sampling_interval_us = 0
    });
    adc_->Initialize();

    sensors_configuration_controller_->Initialize();
    processing_scheduler_service_ = std::make_shared<ProcessingSchedulerService>(time_service_, guid_generator_, adc_, sensors_configuration_controller_);

    processing_scheduler_service_->Start();
    
    return;
}

} // namespace eerie_leap::domain::sensor_domain::services