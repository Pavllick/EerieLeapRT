#pragma once

#include <memory>
#include <string>

#include "domain/sensor_domain/processors/i_reading_processor.h"

namespace eerie_leap::domain::sensor_domain::processors {

// NOTE: calls lua function named according to function_name_ argument
// with reading string sensor id as argument
// and returns float reading value
//
// function process_reading(sensor_id)
//     return 8.1234
// end

class ScriptProcessor : public IReadingProcessor {
private:
    std::string function_name_;

public:
    explicit ScriptProcessor(const std::string& function_name);

    void ProcessReading(std::shared_ptr<SensorReading> reading) override;
};

} // namespace eerie_leap::domain::sensor_domain::processors
