#pragma once

#include <memory>
#include <fstream>

#include "subsys/fs/services/fs_service.h"
#include "subsys/dbc/dbc.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::dbc;

class CanbusController {
private:
    std::shared_ptr<IFsService> fs_service_;
    std::shared_ptr<Dbc> dbc_;

public:
    explicit CanbusController(std::shared_ptr<IFsService> fs_service);

    bool LoadDbcFile(std::streambuf& dbc_content);
    std::shared_ptr<Dbc> GetDbc() const;
};

} // namespace eerie_leap::controllers
