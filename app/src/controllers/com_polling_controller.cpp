#include "com_polling_controller.h"

namespace eerie_leap::controllers {

ComPollingController::ComPollingController(
    std::shared_ptr<ComPollingInterfaceService> com_polling_interface_service,
    std::shared_ptr<LoggingController> logging_controller)
    : com_polling_interface_service_(std::move(com_polling_interface_service)),
      logging_controller_(std::move(logging_controller)) { }

int ComPollingController::Initialize() {
    com_polling_interface_service_->RegisterHandler(ComUserStatus::START_LOGGING, [&logging_controller = logging_controller_]() {
        return StartLoggingHandler(logging_controller);
    });

    com_polling_interface_service_->RegisterHandler(ComUserStatus::STOP_LOGGING, [&logging_controller = logging_controller_]() {
        return StopLoggingHandler(logging_controller);
    });

    return 0;
}

int ComPollingController::StartLoggingHandler(std::shared_ptr<LoggingController> logging_controller) {
    if(logging_controller->SaveLogMetadata() != 0)
        return -1;

    return logging_controller->LogWriterStart();
}

int ComPollingController::StopLoggingHandler(std::shared_ptr<LoggingController> logging_controller) {
    return logging_controller->LogWriterStop();
}

} // namespace eerie_leap::controllers
