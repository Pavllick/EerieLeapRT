#include "work_queue_thread.h"

namespace eerie_leap::subsys::threading {

WorkQueueThread::WorkQueueThread(std::string name, int stack_size, int priority, bool is_cooperative)
    : ThreadBase(stack_size, priority, is_cooperative), name_(std::move(name)) {}

WorkQueueThread::~WorkQueueThread() {
    k_work_queue_stop(&work_q_, K_FOREVER);
}

void WorkQueueThread::Initialize() {
    InitializeStack();

    k_work_queue_init(&work_q_);
    k_work_queue_start(&work_q_, stack_area_, k_stack_size_, k_priority_, nullptr);

    k_thread_name_set(&work_q_.thread, name_.c_str());
}

[[nodiscard]] k_work_q* WorkQueueThread::GetWorkQueue() {
    return &work_q_;
}

void WorkQueueThread::TaskHandler(k_work* work) {
    WorkQueueTaskBase* task = CONTAINER_OF(work, WorkQueueTaskBase, work);
    auto result = task->Execute();

    if(result.reschedule)
        k_work_reschedule_for_queue(task->work_q, &task->work, result.delay);
}

bool WorkQueueThread::CancelTask(WorkQueueTaskBase& task) {
    return k_work_cancel_delayable_sync(&task.work, &sync_);
}

bool WorkQueueThread::FlushTask(WorkQueueTaskBase& task) {
    return k_work_flush_delayable(&task.work, &sync_);
}
} // namespace eerie_leap::subsys::threading
