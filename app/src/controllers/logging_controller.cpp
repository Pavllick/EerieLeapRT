#include "logging_controller.h"

namespace eerie_leap::controllers {

LoggingController::LoggingController(
    std::shared_ptr<LogWriterService> log_writer_service,
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
    std::shared_ptr<LoggingConfigurationManager> logging_configuration_manager,
    std::shared_ptr<DisplayController> display_controller)
        : log_writer_service_(std::move(log_writer_service)),
        sensors_configuration_manager_(std::move(sensors_configuration_manager)),
        logging_configuration_manager_(std::move(logging_configuration_manager)),
        display_controller_(std::move(display_controller)) {

    logger_ = std::make_shared<Mdf4LoggerSensorReading>(
        logging_configuration_manager_,
        *sensors_configuration_manager_->Get());
    log_writer_service_->SetLogger(logger_);
}

int LoggingController::LogWriterStart() {
    int res = log_writer_service_->LogWriterStart();

    if(res == 0)
        display_controller_->AddStatus("log");

    return res;
}

int LoggingController::LogWriterStop() {
    int res = log_writer_service_->LogWriterStop();

    if(res == 0)
        display_controller_->RemoveStatus("log");

    return res;
}

} // namespace eerie_leap::controllers
