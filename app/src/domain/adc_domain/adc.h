#pragma once

#include <stdio.h>

namespace eerie_leap::domain::adc_domain {

class Adc {
public:
    Adc() = default;
    ~Adc() = default;
    
    int Initialize();
    int ReadChannel(int channel);
};

}  // namespace eerie_leap::domain::adc_domain