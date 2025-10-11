#include "views/animations/particles_animation.h"
#include "views/animations/wave_lines_animation.h"
#include "views/animations/tapered_wave_animation.h"
#include "views/animations/dot_path_animation.h"

#include "main_view.h"

namespace eerie_leap::views {

MainView::MainView() {
    cfb_ = std::make_shared<Cfb>();
}

void MainView::Initialize() {
    cfb_->Initialize();

    animation_ = std::make_unique<DotPathAnimation>(cfb_);
    animation_->Initialize();
}

void MainView::Render() {
    cfb_->SetAnimationHandler([this]() {
        animation_->Animate();
    }, 30);
    cfb_->StartAnimation();
}

} // namespace eerie_leap::views
