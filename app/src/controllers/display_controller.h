#pragma once

#include <memory>
#include <set>
#include <string>

#include "subsys/cfb/cfb.h"
#include "views/animations/i_animation.h"

namespace eerie_leap::controllers {

using namespace eerie_leap::subsys::cfb;
using namespace eerie_leap::views::animations;

class DisplayController {
private:
    std::shared_ptr<Cfb> cfb_;
    std::unique_ptr<IAnimation> animation_;
    int current_animation_index_ = 0;
    bool is_animation_in_progress_ = false;
    std::set<std::string> statuses_;
    bool is_initialized_ = false;

    void UpdateStatuses();

public:
    DisplayController(std::shared_ptr<Cfb> cfb);
    void Initialize();

    void StartAnimation(int animation_index = -1);
    void StopAnimation();

    void PrintStringLine(const std::string& str);

    void AddStatus(const std::string& status);
    void RemoveStatus(const std::string& status);
    void ClearStatuses();
};

} // namespace eerie_leap::controllers
