#pragma once

#include <memory>

#include "domain/user_com_domain/services/com_polling/com_polling_interface_service.h"
#include "controllers/logging_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::domain::user_com_domain;
using namespace eerie_leap::domain::user_com_domain::services::com_polling;

class ComPollingController {
private:
    std::shared_ptr<UserCom> user_com_;
    std::shared_ptr<ComPollingInterfaceService> com_polling_interface_service_;
    std::shared_ptr<LoggingController> logging_controller_;

    static int StartLoggingHandler(std::shared_ptr<UserCom> user_com, std::shared_ptr<LoggingController> logging_controller);
    static int StopLoggingHandler(std::shared_ptr<UserCom> user_com, std::shared_ptr<LoggingController> logging_controller);

public:
    ComPollingController(
        std::shared_ptr<UserCom> user_com,
        std::shared_ptr<ComPollingInterfaceService> com_polling_interface_service,
        std::shared_ptr<LoggingController> logging_controller);

    int Initialize();
};

} // namespace eerie_leap::controllers
