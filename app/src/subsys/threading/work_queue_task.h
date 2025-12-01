#pragma once

#include <functional>
#include <memory>
#include <typeindex>

#include <zephyr/kernel.h>

#include "work_queue_task_result.h"

namespace eerie_leap::subsys::threading {

// Type-erased base class
class WorkQueueTaskBase {
public:
    k_work_q* work_q;
    k_work_delayable work;

    virtual ~WorkQueueTaskBase() = default;
    virtual WorkQueueTaskResult Execute() = 0;
};

// Templated derived class that stores the actual type
template<typename T>
class WorkQueueTask : public WorkQueueTaskBase {
public:
    using Handler = std::function<WorkQueueTaskResult(T*)>;

    Handler handler;
    std::unique_ptr<T> user_data;

    WorkQueueTaskResult Execute() override {
        return handler(user_data.get());
    }
};

} // namespace eerie_leap::subsys::threading
