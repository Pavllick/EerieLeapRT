#pragma once

#include <memory>
#include <vector>

#include <zephyr/kernel.h>

#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/canbus_domain/configuration/canbus_configuration_manager.h"
#include "domain/canbus_domain/models/can_message_configuration.h"
#include "domain/canbus_domain/services/canbus_service.h"

#include "canbus_task.hpp"

namespace eerie_leap::domain::canbus_domain::services {

using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::domain::canbus_domain::configuration;

class CanbusSchedulerService {
private:
    k_sem processing_semaphore_;
    static constexpr k_timeout_t PROCESSING_TIMEOUT = K_MSEC(200);

    static constexpr int k_stack_size_ = 4096;
    static constexpr int k_priority_ = K_PRIO_PREEMPT(6);

    k_thread_stack_t* stack_area_;
    k_work_q work_q_;

    std::shared_ptr<CanbusConfigurationManager> canbus_configuration_manager_;
    std::shared_ptr<CanbusService> canbus_service_;
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;

    std::vector<std::shared_ptr<CanbusTask>> tasks_;

    void StartTasks();
    std::shared_ptr<CanbusTask> CreateTask(uint8_t bus_channel, const CanMessageConfiguration& message_configuration);
    static void ProcessCanbusWorkTask(k_work* work);

public:
    CanbusSchedulerService(
        std::shared_ptr<CanbusConfigurationManager> canbus_configuration_manager,
        std::shared_ptr<CanbusService> canbus_service,
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame);
    ~CanbusSchedulerService();

    void Initialize();

    void Start();
    void Restart();
    void Pause();
    void Resume();
};

} // namespace eerie_leap::domain::canbus_domain::services
