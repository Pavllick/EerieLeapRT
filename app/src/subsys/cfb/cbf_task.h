#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

namespace eerie_leap::subsys::cfb {

struct CfbTask {
    atomic_t is_animation_running_;
    std::function<void()> animation_handler;
    uint32_t frame_rate;

    CfbTask() : is_animation_running_(ATOMIC_INIT(0)) {}
};

} // namespace eerie_leap::subsys::cfb
