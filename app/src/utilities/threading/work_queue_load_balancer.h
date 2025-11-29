#pragma once

#include <vector>
#include <memory>

#include <zephyr/kernel.h>
#include <zephyr/sys/mutex.h>

#include "work_queue_thread.h"
#include "work_queue_load_metrics.h"

namespace eerie_leap::utilities::threading {

class WorkQueueLoadBalancer {
private:
    std::vector<std::shared_ptr<WorkQueueThread>> work_queue_threads_;
    std::vector<WorkQueueLoadMetrics> thread_metrics_;
    k_mutex balancer_mutex_;

public:
    WorkQueueLoadBalancer();
    ~WorkQueueLoadBalancer() = default;

    void AddThread(std::shared_ptr<WorkQueueThread> thread);
    std::shared_ptr<WorkQueueThread> GetLeastLoadedQueue();
    void OnWorkComplete(WorkQueueThread& thread, uint32_t execution_time_ms);
};

} // namespace eerie_leap::utilities::threading
