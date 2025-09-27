#include "logging_controller.h"

namespace eerie_leap::controllers {

LoggingController::LoggingController(
    std::shared_ptr<LogWriterService> log_writer_service,
    std::shared_ptr<SensorsConfigurationController> sensors_configuration_controller)
    : log_writer_service_(std::move(log_writer_service)),
    sensors_configuration_controller_(std::move(sensors_configuration_controller)) { }

int LoggingController::LogWriterStart() {
    int res = log_writer_service_->SaveLogMetadata(sensors_configuration_controller_->GetRaw());

    if(res == 0)
        res = log_writer_service_->LogWriterStart();

    return res;
}

int LoggingController::LogWriterStop() {
    return log_writer_service_->LogWriterStop();
}

} // namespace eerie_leap::controllers
