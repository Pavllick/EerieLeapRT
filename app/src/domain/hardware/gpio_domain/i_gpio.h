#pragma once

namespace eerie_leap::domain::hardware::gpio_domain {

class IGpio {
public:
    virtual int Initialize() = 0;
    virtual bool ReadChannel(int channel) = 0;
    virtual int GetChannelCount() = 0;
};

}  // namespace eerie_leap::domain::hardware::gpio_domain
