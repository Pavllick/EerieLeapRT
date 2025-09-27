#pragma once

#include <memory>

#include "domain/logging_domain/services/log_writer_service.h"
#include "controllers/sensors_configuration_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::domain::logging_domain::services;

class LoggingController {
private:
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller_;
    std::shared_ptr<LogWriterService> log_writer_service_;

public:
    LoggingController(
        std::shared_ptr<LogWriterService> log_writer_service,
        std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller);

    int LogWriterStart();
    int LogWriterStop();
};

} // namespace eerie_leap::controllers
