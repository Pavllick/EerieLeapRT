#pragma once

#include <memory>

#include "utilities/memory/heap_allocator.h"
#include "subsys/cfb/cfb.h"
#include "views/animations/i_animation.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::cfb;
using namespace eerie_leap::views::animations;

class DisplayController {
private:
    std::shared_ptr<Cfb> cfb_;
    ext_unique_ptr<IAnimation> animation_;
    int current_animation_index_ = 0;
    bool is_animation_in_progress_ = false;

public:
    DisplayController(std::shared_ptr<Cfb> cfb);
    void Initialize();

    bool StartAnimation(int animation_index = -1);
    bool StopAnimation();

    bool PrintStringLine(const std::string& str);
};

} // namespace eerie_leap::controllers
