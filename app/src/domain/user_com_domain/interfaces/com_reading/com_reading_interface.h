#pragma once

#include <memory>

#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/user_com_domain/user_com.h"

#include "com_reading_task.hpp"

namespace eerie_leap::domain::user_com_domain::interfaces::com_reading {

using namespace eerie_leap::domain::sensor_domain::models;

class ComReadingInterface {
private:
    static constexpr int k_stack_size_ = 1024;
    static constexpr int k_priority_ = K_PRIO_COOP(10);

    static z_thread_stack_element stack_area_[k_stack_size_];
    k_work_q work_q;
    std::shared_ptr<ComReadingTask> task_;

    k_sem processing_semaphore_;
    static constexpr k_timeout_t SENDING_TIMEOUT = K_MSEC(200);

    static void SendReadingWorkTask(k_work* work);

    std::shared_ptr<UserCom> user_com_;

public:
    explicit ComReadingInterface(std::shared_ptr<UserCom> user_com);
    void Initialize();

    int SendReading(std::shared_ptr<SensorReading> reading, uint8_t user_id = Modbus::SERVER_ID_ALL);
};

} // namespace eerie_leap::domain::user_com_domain::interfaces::com_reading
