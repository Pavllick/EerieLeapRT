#pragma once

#include <fstream>
#include <sstream>

namespace eerie_leap::subsys::dbc {

class DbcTestData {
public:
    DbcTestData() = delete;
    ~DbcTestData() = delete;

    static std::stringstream GetDbcContent();
};

} // namespace eerie_leap::subsys::dbc
