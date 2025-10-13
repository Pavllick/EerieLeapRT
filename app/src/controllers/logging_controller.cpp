#include "logging_controller.h"

namespace eerie_leap::controllers {

LoggingController::LoggingController(
    std::shared_ptr<LogWriterService> log_writer_service,
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
    std::shared_ptr<DisplayController> display_controller)
        : log_writer_service_(std::move(log_writer_service)),
        sensors_configuration_manager_(std::move(sensors_configuration_manager)),
        display_controller_(std::move(display_controller)) { }

int LoggingController::LogWriterStart() {
    int res = log_writer_service_->SaveLogMetadata(sensors_configuration_manager_->GetRaw());

    if(res == 0)
        res = log_writer_service_->LogWriterStart();

    if(res == 0)
        display_controller_->PrintStringLine("Logging in progress");

    return res;
}

int LoggingController::LogWriterStop() {
    int res = log_writer_service_->LogWriterStop();

    if(res == 0)
        display_controller_->StartAnimation();

    return res;
}

} // namespace eerie_leap::controllers
