#include <memory>
#include <stdexcept>

#include "domain/sensor_domain/models/reading_status.h"
#include "domain/sensor_domain/models/reading_metadata.h"

#include "can_frame_dbc_builder.h"

namespace eerie_leap::domain::canbus_domain::can_frame_builders {

using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::utilities;
using namespace eerie_leap::utilities::voltage_interpolator;

CanFrameDbcBuilder::CanFrameDbcBuilder(
    std::shared_ptr<CanbusService> canbus_service,
    std::shared_ptr<SensorReadingsFrame> sensor_readings_frame)
        : canbus_service_(std::move(canbus_service)),
        sensor_readings_frame_(std::move(sensor_readings_frame)) {}

std::vector<uint8_t> CanFrameDbcBuilder::Build(uint8_t bus_channel, uint32_t frame_id) {
    try {
        auto dbc = canbus_service_->GetChannelConfiguration(bus_channel)->dbc;

        if(dbc == nullptr)
            throw std::runtime_error("DBC not loaded");

        auto can_frame_data = dbc->GetMessage(frame_id)->EncodeMessage(
            [&sensor_readings_frame = sensor_readings_frame_](size_t signal_name_hash) {
                if(!sensor_readings_frame->HasReadingValue(signal_name_hash))
                    return 0.0f;

                return sensor_readings_frame->GetReadingValue(signal_name_hash);
            });

        return can_frame_data;
    } catch (const std::exception& e) {
        return {};
    }
}

} // namespace eerie_leap::domain::canbus_domain::can_frame_builders
