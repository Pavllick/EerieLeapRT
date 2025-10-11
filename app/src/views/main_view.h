#pragma once

#include <memory>

#include "subsys/cfb/cfb.h"
#include "views/animations/i_animation.h"

namespace eerie_leap::views {

using namespace eerie_leap::subsys::cfb;
using namespace eerie_leap::views::animations;

class MainView {
private:
    std::shared_ptr<Cfb> cfb_;
    std::unique_ptr<IAnimation> animation_;

public:
    MainView();

    void Initialize();
    void Render();
};

}  // namespace eerie_leap::views
