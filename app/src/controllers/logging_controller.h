#pragma once

#include <memory>

#include "domain/logging_domain/services/log_writer_service.h"
#include "domain/sensor_domain/configuration/sensors_configuration_manager.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::domain::sensor_domain::configuration;
using namespace eerie_leap::domain::logging_domain::services;

class LoggingController {
private:
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager_;
    std::shared_ptr<LogWriterService> log_writer_service_;

public:
    LoggingController(
        std::shared_ptr<LogWriterService> log_writer_service,
        std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager);

    int LogWriterStart();
    int LogWriterStop();
};

} // namespace eerie_leap::controllers
