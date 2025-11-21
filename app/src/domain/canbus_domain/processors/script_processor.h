#pragma once

#include <string>

#include "domain/canbus_domain/services/canbus_service.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/canbus_domain/processors/i_can_frame_processor.h"

namespace eerie_leap::domain::canbus_domain::processors {

using namespace eerie_leap::domain::canbus_domain::services;
using namespace eerie_leap::domain::sensor_domain::utilities;

// NOTE: calls lua function named according to function_name_ argument
// with CAN frame ID and CAN frame byte array as arguments
// and returns byte array
//
// function process_can_frame(frame_id, data)
//     -- Convert string to table of bytes for easier manipulation
//     local bytes = {string.byte(data, 1, -1)}

//     -- Example: Modify some bits
//     bytes[1] = bytes[1] | 0x80  -- Set bit 7 of first byte
//     bytes[2] = bytes[2] & 0x0F  -- Clear upper nibble of second byte

//     -- Convert back to string
//     return string.char(table.unpack(bytes))
// end

class ScriptProcessor : public ICanFrameProcessor {
private:
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame_;
    std::string function_name_;

public:
    ScriptProcessor(
        std::shared_ptr<SensorReadingsFrame> sensor_readings_frame,
        std::string function_name);

    CanFrame Process(
        const CanMessageConfiguration& message_configuration,
        const CanFrame& can_frame) override;
};

} // namespace eerie_leap::domain::canbus_domain::processors
