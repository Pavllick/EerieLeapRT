#pragma once

#include <zephyr/zbus/zbus.h>
#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::events {

using namespace eerie_leap::domain::sensor_domain::models;

// TODO: Replace SensorReading complex type with a simple DTO that can be hard copied
// and passed through the ZBUS channel. This is to avoid the need for dynamic memory
// allocation and to ensure that the data can be passed through the ZBUS channel
// without any issues. The current implementation may lead to memory leaks or
// undefined behavior if the SensorReading object is not properly managed.

// ZBUS_CHAN_DEFINE(
//     sensor_reading_channel,
//     SensorReading,
//     NULL,
//     NULL,
//     ZBUS_OBSERVERS(sensor_reading_observer_canbus),
//     ZBUS_MSG_INIT()
// );

} // namespace eerie_leap::domain::sensor_domain::events