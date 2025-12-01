#pragma once

#include <vector>
#include <memory>

#include <zephyr/kernel.h>
#include <zephyr/sys/mutex.h>

#include "work_queue_thread.h"
#include "work_queue_load_metrics.h"

namespace eerie_leap::subsys::threading {

// NOTE: Usage example:
//
// auto load_balancer = std::make_shared<WorkQueueLoadBalancer>();
// load_balancer->AddThread(thread1);
// load_balancer->AddThread(thread2);
// load_balancer->AddThread(thread3);
// ...
//
// void ProcessWorkTask(k_work* work) {
//     WorkTask* task = CONTAINER_OF(work, WorkTask, work);
//     uint32_t start_time = k_uptime_get_32();
//
//     // do work
//
//     task->load_balancer->OnWorkComplete(*task->work_queue_thread_, k_uptime_get_32() - start_time);
//     auto work_queue_thread = task->load_balancer->GetLeastLoadedQueue();
//     task->work_queue_thread_ = work_queue_thread;
//     k_work_reschedule_for_queue(work_queue_thread->GetWorkQueue(), &work, task->sampling_rate_ms);
// }
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

} // namespace eerie_leap::subsys::threading
