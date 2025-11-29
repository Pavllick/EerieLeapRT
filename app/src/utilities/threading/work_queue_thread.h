#pragma once

#include <string>

#include "thread_base.h"

namespace eerie_leap::utilities::threading {

class WorkQueueThread : public ThreadBase {
private:
    k_work_q work_q_;
    std::string name_;

public:
    WorkQueueThread(std::string name, int stack_size, int priority, bool is_cooperative = false);
    ~WorkQueueThread();

    void Initialize();
    [[nodiscard]] k_work_q* GetWorkQueue();
};

} // namespace eerie_leap::utilities::threading
