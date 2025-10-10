#pragma once

#include <memory>
#include <queue>

#include <zephyr/kernel.h>

namespace eerie_leap::subsys::cfb {

struct CfbTask {
    k_work work;
    k_work_q* work_q;
    k_sem* processing_semaphore;
};

} // namespace eerie_leap::subsys::cfb
