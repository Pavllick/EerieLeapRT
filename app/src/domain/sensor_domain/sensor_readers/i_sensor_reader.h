#pragma once

namespace eerie_leap::domain::sensor_domain::sensor_readers {

class ISensorReader {
public:
    virtual ~ISensorReader() = default;

    virtual void Read() = 0;
};

} // namespace eerie_leap::domain::sensor_domain::sensor_readers
