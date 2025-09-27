#include "domain/user_com_domain/types/com_user_status.h"

#include "com_polling_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::domain::user_com_domain::types;

ComPollingController::ComPollingController(
    std::shared_ptr<UserCom> user_com,
    std::shared_ptr<ComPollingInterfaceService> com_polling_interface_service,
    std::shared_ptr<LoggingController> logging_controller)
    : user_com_(std::move(user_com)),
      com_polling_interface_service_(std::move(com_polling_interface_service)),
      logging_controller_(std::move(logging_controller)) { }

int ComPollingController::Initialize() {
    com_polling_interface_service_->RegisterHandler(ComUserStatus::START_LOGGING, [&user_com = user_com_, &logging_controller = logging_controller_]() {
        return StartLoggingHandler(user_com, logging_controller);
    });

    com_polling_interface_service_->RegisterHandler(ComUserStatus::STOP_LOGGING, [&user_com = user_com_, &logging_controller = logging_controller_]() {
        return StopLoggingHandler(user_com, logging_controller);
    });

    return 0;
}

int ComPollingController::StartLoggingHandler(std::shared_ptr<UserCom> user_com, std::shared_ptr<LoggingController> logging_controller) {
    int res = logging_controller->LogWriterStart();

    if(res == 0)
        res = user_com->StatusUpdateNotify(ComUserStatus::START_LOGGING, true);
    else
        user_com->StatusUpdateNotify(ComUserStatus::START_LOGGING, false);

    return res;
}

int ComPollingController::StopLoggingHandler(std::shared_ptr<UserCom> user_com, std::shared_ptr<LoggingController> logging_controller) {
    int res = logging_controller->LogWriterStop();

    if(res == 0)
        res = user_com->StatusUpdateNotify(ComUserStatus::STOP_LOGGING, true);
    else
        user_com->StatusUpdateNotify(ComUserStatus::STOP_LOGGING, false);

    return res;
}

} // namespace eerie_leap::controllers
