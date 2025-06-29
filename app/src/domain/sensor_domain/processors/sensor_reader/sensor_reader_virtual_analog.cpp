#include <stdexcept>

#include "utilities/memory/heap_allocator.h"
#include "domain/sensor_domain/models/sensor_reading.h"
#include "domain/sensor_domain/models/reading_status.h"
#include "sensor_reader_virtual_analog.h"

namespace eerie_leap::domain::sensor_domain::processors::sensor_reader {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::hardware::adc_domain::utilities;

SensorReaderVirtualAnalog::SensorReaderVirtualAnalog(
    std::shared_ptr<ITimeService>& time_service,
    std::shared_ptr<GuidGenerator>& guid_generator,
    std::shared_ptr<SensorReadingsFrame>& readings_frame,
    std::shared_ptr<Sensor>& sensor)
    : SensorReaderBase(time_service, guid_generator, readings_frame, sensor) {

    if(sensor_->configuration.type != SensorType::VIRTUAL_ANALOG)
        throw std::runtime_error("Unsupported sensor type");
}

void SensorReaderVirtualAnalog::Read() {
    auto reading = make_shared_ext<SensorReading>(guid_generator_->Generate(), sensor_);
    reading->timestamp = time_service_->GetCurrentTime();

    reading->status = ReadingStatus::UNINITIALIZED;

    readings_frame_->AddOrUpdateReading(reading);
}

} // namespace eerie_leap::domain::sensor_domain::processors::sensor_reader
