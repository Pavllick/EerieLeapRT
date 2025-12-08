#pragma once

namespace eerie_leap::subsys::gpio {

class IGpio {
public:
    virtual ~IGpio() = default;

    virtual int Initialize() = 0;
    virtual bool ReadChannel(int channel) = 0;
    virtual int GetChannelCount() = 0;
};

}  // namespace eerie_leap::subsys::gpio
