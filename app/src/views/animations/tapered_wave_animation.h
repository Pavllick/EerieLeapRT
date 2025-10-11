#pragma once

#include <memory>
#include <cmath>

#include "subsys/cfb/cfb.h"

#include "i_animation.h"

namespace eerie_leap::views::animations {

using namespace eerie_leap::subsys::cfb;

class TaperedWaveAnimation : public IAnimation {
private:
    std::shared_ptr<Cfb> cfb_;
    float time_;

    static constexpr float THICKNESS = 14.0F;

    void DrawThickLine(Coordinate p1, Coordinate p2, float thickness);

public:
    explicit TaperedWaveAnimation(std::shared_ptr<Cfb> cfb);

    void Initialize() override;
    void Animate() override;
};

} // namespace eerie_leap::views::animations
