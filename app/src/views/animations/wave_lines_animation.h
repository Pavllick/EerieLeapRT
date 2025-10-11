#pragma once

#include <cmath>
#include <memory>

#include "subsys/cfb/cfb.h"

#include "i_animation.h"

namespace eerie_leap::views::animations {

using namespace eerie_leap::subsys::cfb;

class WaveLinesAnimation : public IAnimation {
private:
    std::shared_ptr<Cfb> cfb_;
    uint32_t frame_;

    static constexpr int LINES_COUNT = 3;
    static constexpr int WAVE_COUNT = 8; // Number of wave cycles

    void DrawWaveLine(float t, int line_index);

public:
    explicit WaveLinesAnimation(std::shared_ptr<Cfb> cfb);

    void Initialize() override;
    void Animate() override;
};

} // namespace eerie_leap::views::animations
