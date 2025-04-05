#pragma once

#include <zephyr/kernel.h>

namespace eerie_leap::domain::sensor_domain::services {

class MeasurementService {
private:
    static constexpr int kStackSize = 500;
    static constexpr int kPriority = K_PRIO_PREEMPT(8);
    static void ThreadTrampoline(void* instance, void* p2, void* p3);

    K_KERNEL_STACK_MEMBER(stack_area, kStackSize);

    k_tid_t thread_id_;
    k_thread thread_data_;
    void EntryPoint();

public:
    k_tid_t Start();
};

} // namespace eerie_leap::domain::sensor_domain::services