#include <stdio.h>
#include <zephyr/kernel.h>

#include "domain/sensor_domain/services/measurement_service.h"

using namespace eerie_leap::domain::sensor_domain::services;

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