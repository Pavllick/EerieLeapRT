#include "thread_base.h"

namespace eerie_leap::subsys::threading {

ThreadBase::ThreadBase(int stack_size, int priority, bool is_cooperative) : k_stack_size_(stack_size) {
    k_priority_ = is_cooperative ? K_PRIO_COOP(priority) : K_PRIO_PREEMPT(priority);
}

ThreadBase::~ThreadBase() {
    if(stack_area_ == nullptr)
        return;

    k_thread_stack_free(stack_area_);
}

void ThreadBase::InitializeStack() {
    stack_area_ = k_thread_stack_alloc(k_stack_size_, 0);
}

[[nodiscard]] const k_thread_stack_t* ThreadBase::GetStack() const {
    return stack_area_;
}

} // namespace eerie_leap::subsys::threading
