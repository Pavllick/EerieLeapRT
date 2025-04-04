#include <stdio.h>
#include <zephyr/kernel.h>

#include "domain/adc_domain/services/measurement_service.h"

using namespace eerie_leap::domain::adc_domain::services;

#define SLEEP_TIME_MS 3000

int main(void) {
    MeasurementService measurement_service;
    measurement_service.Start();

    while (true) {
        printf("Hello world\n");
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}