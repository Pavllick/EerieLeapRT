#pragma once

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

class ISensorReader {
public:
    virtual void Read() = 0;
};

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
