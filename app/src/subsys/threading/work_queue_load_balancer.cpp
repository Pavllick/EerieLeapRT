#include "work_queue_load_balancer.h"

namespace eerie_leap::subsys::threading {

WorkQueueLoadBalancer::WorkQueueLoadBalancer() {
    k_mutex_init(&balancer_mutex_);
}

void WorkQueueLoadBalancer::AddThread(std::shared_ptr<WorkQueueThread> thread) {
    work_queue_threads_.emplace_back(std::move(thread));
    thread_metrics_.emplace_back(work_queue_threads_.back()->GetWorkQueue());
}

void WorkQueueLoadBalancer::OnWorkComplete(WorkQueueThread& thread, uint32_t execution_time_ms) {
    int index = 0;
    for(const auto& t : work_queue_threads_) {
        if(t->GetWorkQueue() == thread.GetWorkQueue())
            break;

        index++;
    }

    thread_metrics_[index].OnWorkComplete(execution_time_ms);
}

std::shared_ptr<WorkQueueThread> WorkQueueLoadBalancer::GetLeastLoadedQueue() {
    k_mutex_lock(&balancer_mutex_, K_FOREVER);

    uint64_t now = k_uptime_get();
    int min_score = INT_MAX;
    size_t least_loaded_index = 0;

    for(size_t i = 0; i < work_queue_threads_.size(); i++) {
        // Decay old load over time
        uint64_t time_delta = now - thread_metrics_[i].last_update_ms;
        if(time_delta > 1000) {  // Decay after 1 second
            atomic_set(&thread_metrics_[i].total_load_ms,
                atomic_get(&thread_metrics_[i].total_load_ms) / 2);
            thread_metrics_[i].last_update_ms = now;
        }

        // Score = pending items * avg_time_per_item
        int pending = atomic_get(&thread_metrics_[i].pending_items);
        int total_load = atomic_get(&thread_metrics_[i].total_load_ms);
        int score = pending * 10 + total_load;  // Weight pending items more

        if(score < min_score) {
            min_score = score;
            least_loaded_index = i;
        }
    }

    atomic_inc(&thread_metrics_[least_loaded_index].pending_items);
    k_mutex_unlock(&balancer_mutex_);

    return work_queue_threads_[least_loaded_index];
}

} // namespace eerie_leap::subsys::threading
