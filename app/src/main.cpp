#include <stdio.h>
#include <zephyr/kernel.h>

#include "domain/adc_domain/controllers/measurement_processor.h"

using namespace eerie_leap::domain::adc_domain::controllers;

#define SLEEP_TIME_MS 3000

int main(void) {
    MeasurementProcessor measurement_processor;
    measurement_processor.Start();

    while (true) {
        printf("Hello world\n");
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}