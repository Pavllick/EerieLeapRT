#include "domain/canbus_com_domain/commands/canbus_com_logging_command.h"

#include "logging_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::domain::canbus_com_domain::commands;

LoggingController::LoggingController(
    std::shared_ptr<LogWriterService> log_writer_service,
    std::shared_ptr<SensorsConfigurationManager> sensors_configuration_manager,
    std::shared_ptr<LoggingConfigurationManager> logging_configuration_manager,
    std::shared_ptr<CanbusComService> canbus_com_service,
    std::shared_ptr<DisplayController> display_controller)
        : log_writer_service_(std::move(log_writer_service)),
        sensors_configuration_manager_(std::move(sensors_configuration_manager)),
        logging_configuration_manager_(std::move(logging_configuration_manager)),
        canbus_com_service_(std::move(canbus_com_service)),
        display_controller_(std::move(display_controller)) {

    logger_ = std::make_shared<Mdf4LoggerSensorReading>(
        logging_configuration_manager_,
        *sensors_configuration_manager_->Get());
    log_writer_service_->SetLogger(logger_);

    canbus_com_service_->SetCommandHandler<CanbusComLoggingCommand>(
        CanbusComCommandCode::LOGGING,
        [this](std::optional<CanbusComLoggingCommand> command) {
            if(!command.has_value())
                return false;

            if(command.value().IsStart())
                return LogWriterStart() == 0;
            else
                return LogWriterStop() == 0;
        });
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
