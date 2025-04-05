#pragma once

#include <zephyr/zbus/zbus.h>
#include "domain/sensor_domain/models/sensor_reading.h"

namespace eerie_leap::domain::sensor_domain::events {

using namespace eerie_leap::domain::sensor_domain::models;

ZBUS_CHAN_DEFINE(
    sensor_reading_channel,
    SensorReading,
    NULL,
    NULL,
    ZBUS_OBSERVERS(sensor_reading_observer_canbus),
    ZBUS_MSG_INIT()
);

} // namespace eerie_leap::domain::sensor_domain::events