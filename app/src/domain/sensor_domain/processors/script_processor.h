#pragma once

#include <memory>
#include <string>

#include "domain/sensor_domain/processors/i_reading_processor.h"

namespace eerie_leap::domain::sensor_domain::processors {

class ScriptProcessor : public IReadingProcessor {
private:
    std::string function_name_;

public:
    explicit ScriptProcessor(const std::string& function_name);

    void ProcessReading(std::shared_ptr<SensorReading> reading) override;
};

} // namespace eerie_leap::domain::sensor_domain::processors
