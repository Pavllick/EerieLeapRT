#pragma once

#include <zephyr/kernel.h>

namespace eerie_leap::subsys::threading {

struct WorkQueueLoadMetrics {
    k_work_q* work_queue;
    atomic_t pending_items;
    atomic_t total_load_ms;
    uint64_t last_update_ms;

    WorkQueueLoadMetrics(k_work_q* work_queue)
        : work_queue(work_queue),
        pending_items(ATOMIC_INIT(0)),
        total_load_ms(ATOMIC_INIT(0)),
        last_update_ms(k_uptime_get()) {}

    void OnWorkComplete(uint32_t execution_time_ms) {
        atomic_dec(&pending_items);
        atomic_add(&total_load_ms, execution_time_ms);
    }
};

} // namespace eerie_leap::subsys::threading
