#include <zephyr/logging/log.h>

#include "utilities/memory/heap_allocator.h"
#include "subsys/fs/services/fs_service_stream_buf.h"

#include "canbus_controller.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::utilities::memory;

LOG_MODULE_REGISTER(canbus_controller_logger);

CanbusController::CanbusController(std::shared_ptr<IFsService> fs_service)
    : fs_service_(std::move(fs_service)) {

    dbc_ = make_shared_ext<Dbc>();

    if(fs_service_ != nullptr && fs_service_->Exists(CONFIG_EERIE_LEAP_CANBUS_DBC_FILE_PATH)) {
        FsServiceStreamBuf fs_stream_buf(
            fs_service_.get(),
            CONFIG_EERIE_LEAP_CANBUS_DBC_FILE_PATH,
            FsServiceStreamBuf::OpenMode::Read);
        LoadDbcFile(fs_stream_buf);

        fs_stream_buf.close();
    }
}

bool CanbusController::LoadDbcFile(std::streambuf& dbc_content) {
    bool res = dbc_->LoadDbcFile(dbc_content);
    if(res)
        LOG_INF("DBC file loaded successfully.");
    else
        LOG_ERR("Failed to load DBC file.");

    return res;
}

std::shared_ptr<Dbc> CanbusController::GetDbc() const {
    return dbc_;
}

} // namespace eerie_leap::controllers
