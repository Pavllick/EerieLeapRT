#include <stdio.h>
#include <zephyr/kernel.h>

#include "threads/measurement_processing_thread.h"

using namespace eerie_leap::threads;

#define SLEEP_TIME_MS 3000

int main(void) {
    MeasurementProcessingThread measurement_processing_thread;
    measurement_processing_thread.Start();

    while (true) {
        printf("Hello world\n");
        k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}