#pragma once

#include <vector>
#include <string>

#include "thread_base.h"
#include "work_queue_task.h"

namespace eerie_leap::subsys::threading {

class WorkQueueThread : public ThreadBase {
private:
    k_work_q work_q_;
    std::string name_;
    k_work_sync sync_;

    static void TaskHandler(k_work* work);

public:
    WorkQueueThread(std::string name, int stack_size, int priority, bool is_cooperative = false);
    ~WorkQueueThread();

    void Initialize();
    [[nodiscard]] k_work_q* GetWorkQueue();

    template<typename T>
    WorkQueueTask<T> CreateTask(const WorkQueueTask<T>::Handler& handler, std::unique_ptr<T> user_data) {
        WorkQueueTask<T> task;
        task.work_q = &work_q_;
        task.handler = handler;
        task.user_data = std::move(user_data);
        k_work_init_delayable(&task.work, TaskHandler);

        return task;
    }

    void ScheduleTask(WorkQueueTaskBase& task) {
        k_work_schedule_for_queue(&work_q_, &task.work, K_NO_WAIT);
    }

    bool CancelTask(WorkQueueTaskBase& task);
    bool FlushTask(WorkQueueTaskBase& task);
};

} // namespace eerie_leap::subsys::threading
