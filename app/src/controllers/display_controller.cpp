#include "views/animations/particles_animation.h"
#include "views/animations/wave_lines_animation.h"
#include "views/animations/tapered_wave_animation.h"
#include "views/animations/dot_path_animation.h"

#include "display_controller.h"

namespace eerie_leap::controllers {

DisplayController::DisplayController(std::shared_ptr<Cfb> cfb)
    : cfb_(std::move(cfb)), current_animation_index_(0) { }

void DisplayController::Initialize() {
    StartAnimation(0);
}

bool DisplayController::StartAnimation(int animation_index) {
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
            return false;
    }

    animation_->Initialize();

    cfb_->SetAnimationHandler([this]() {
        animation_->Animate();
    }, 30);
    cfb_->StartAnimation();

    is_animation_in_progress_ = true;
    current_animation_index_ = animation_index;

    return true;
}

bool DisplayController::StopAnimation() {
    cfb_->StopAnimation();

    animation_.reset();
    is_animation_in_progress_ = false;

    return true;
}

bool DisplayController::PrintStringLine(const std::string& str) {
    if(is_animation_in_progress_)
        StopAnimation();

    cfb_->Clear();
    cfb_->PrintString(str.c_str(), {0, 0});
    cfb_->Flush();

    return true;
}

} // namespace eerie_leap::controllers
