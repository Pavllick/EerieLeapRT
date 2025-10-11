#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

namespace eerie_leap::subsys::cfb {

struct CfbTask {
    k_work work;
    k_work_q* work_q;
    k_sem* processing_semaphore;
    atomic_t is_animation_running_;
    std::function<void()> animation_handler;
    uint32_t frame_rate;
};

} // namespace eerie_leap::subsys::cfb
