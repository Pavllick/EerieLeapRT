#include <stdio.h>
#include <zephyr/kernel.h>

#define SLEEP_TIME_MS 3000

int main(void)
{
    while (true) {
        printf("Hello world\n");
		k_msleep(SLEEP_TIME_MS);
    }

    return 0;
}