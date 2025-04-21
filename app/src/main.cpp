#include <stdio.h>

#include "domain/sensor_domain/services/measurement_service.h"

using namespace eerie_leap::domain::sensor_domain::services;

#define SLEEP_TIME_MS 3000

int main(void) {
    // Placement-new construction of MeasurementService in statically allocated,
    // properly aligned memory. This ensures the internal Zephyr thread stack
    // (defined via K_KERNEL_STACK_MEMBER) is in valid memory and survives for
    // the lifetime of the thread.
    //
    // DO NOT allocate this on the heap or stack — it will crash due to stack
    // alignment or lifetime issues in Zephyr.
    alignas(ARCH_STACK_PTR_ALIGN) static uint8_t service_buffer[sizeof(MeasurementService)];
    auto* service = new (service_buffer) MeasurementService();
    service->Start();

    while (true) {
        printf("Hello world\n");
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}