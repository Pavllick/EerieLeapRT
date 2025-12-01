#pragma once

#include <zephyr/kernel.h>

namespace eerie_leap::subsys::threading {

struct WorkQueueTaskResult {
    bool reschedule = false;
    k_timeout_t delay = K_NO_WAIT;
};

} // namespace eerie_leap::subsys::threading
