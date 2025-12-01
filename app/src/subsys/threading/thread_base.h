#pragma once

#include <zephyr/kernel.h>

namespace eerie_leap::subsys::threading {

class ThreadBase {
protected:
    int k_stack_size_;
    int k_priority_;
    k_thread_stack_t* stack_area_;

public:
    ThreadBase(int stack_size, int priority, bool is_cooperative = false);
    ~ThreadBase();

    void InitializeStack();
    [[nodiscard]] const k_thread_stack_t* GetStack() const;
};

} // namespace eerie_leap::subsys::threading
