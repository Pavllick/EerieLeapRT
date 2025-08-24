#pragma once

namespace eerie_leap::domain::device_tree {

class DtConfigurator {
private:
    DtConfigurator() = default;

public:
    static void Initialize();
};

} // namespace eerie_leap::domain::device_tree
