#include "views/animations/particles_animation.h"
#include "views/animations/wave_lines_animation.h"
#include "views/animations/tapered_wave_animation.h"
#include "views/animations/dot_path_animation.h"

#include "display_controller.h"

namespace eerie_leap::controllers {

DisplayController::DisplayController(std::shared_ptr<Cfb> cfb)
    : cfb_(std::move(cfb)), current_animation_index_(0) { }

void DisplayController::Initialize() {
    if(!cfb_ || is_initialized_)
        return;

    cfb_->SetFont(0);
    is_initialized_ = true;

    StartAnimation(0);
}

void DisplayController::StartAnimation(int animation_index) {
    if(!is_initialized_)
        return;

    if(animation_index < 0)
        animation_index = current_animation_index_;

    switch(animation_index) {
        case 0:
            animation_ = make_unique_ext<WaveLinesAnimation>(cfb_);
            break;
        case 1:
            animation_ = make_unique_ext<DotPathAnimation>(cfb_);
            break;
        case 2:
            animation_ = make_unique_ext<ParticlesAnimation>(cfb_);
            break;
        case 3:
            animation_ = make_unique_ext<TaperedWaveAnimation>(cfb_);
            break;
        default:
            return;
    }

    animation_->Initialize();

    cfb_->SetAnimationHandler([this]() {
        animation_->Animate();
    }, 30);
    cfb_->StartAnimation();

    is_animation_in_progress_ = true;
    current_animation_index_ = animation_index;
}

void DisplayController::StopAnimation() {
    if(!is_initialized_)
        return;

    if(!is_animation_in_progress_)
        return;

    cfb_->StopAnimation();

    animation_.reset();
    is_animation_in_progress_ = false;
}

void DisplayController::PrintStringLine(const std::string& str) {
    if(!is_initialized_)
        return;

    StopAnimation();

    cfb_->Clear();
    cfb_->PrintString(str.c_str(), {0, 0});
    cfb_->Flush();
}

void DisplayController::AddStatus(const std::string& status) {
    if(!is_initialized_)
        return;

    statuses_.insert(status);
    UpdateStatuses();
}

void DisplayController::RemoveStatus(const std::string& status) {
    if(!is_initialized_)
        return;

    statuses_.erase(status);
    UpdateStatuses();
}

void DisplayController::ClearStatuses() {
    if(!is_initialized_)
        return;

    statuses_.clear();
    UpdateStatuses();
}

void DisplayController::UpdateStatuses() {
    if(!is_initialized_)
        return;

    if(statuses_.empty()) {
        StartAnimation();

        return;
    }

    int length_chars = 0;
    for(const auto& s : statuses_)
        length_chars += s.size();

    int padding = 1;

    int length_px = length_chars * cfb_->GetFontWidth() + statuses_.size() * padding * 2;

    int start_x_px = cfb_->GetXRes() - length_px;
    start_x_px = start_x_px < 0 ? 0 : start_x_px;
    int y = cfb_->GetYRes() - cfb_->GetFontHeight() - padding;

    StopAnimation();
    cfb_->Clear();

    int current_x_px = start_x_px;
    int i = 0;
    for(const auto& s : statuses_) {
        cfb_->PrintStringLine(s.c_str(), {current_x_px + padding, y});
        int current_length_px = s.size() * cfb_->GetFontWidth() + padding * 2;

        if(i++ % 2 != 0) {
            cfb_->InvertArea(
                {current_x_px, y - padding},
                {current_x_px + current_length_px, y + cfb_->GetFontHeight() + padding});
        }

        current_x_px += current_length_px;
    }

    cfb_->Flush();
}

} // namespace eerie_leap::controllers
