# Lua Script API

Scripts can be assigned to Sensors and CAN Messages. Each has different API methods available. This documentation describes the API available for each type.

## Common API

These methods are available from both Sensor and CAN Message scripts.

---

`float get_sensor_value(string sensor_id)`

Returns the value of the sensor with the given ID, or `nil` if the sensor value is not available.

---

`void update_sensor_value(string sensor_id, float value)`

Updates the value of the sensor with the given ID.

## Sensor API

### Functions user can define

These methods can be defined in the script. They will be called at different stages of the sensor processing.

---

`float create_sensor_value(string sensor_id)`

Only applicable for USER_ANALOG and USER_INDICATOR sensor types.

This function will be called to generate the initial sensor value, as if it were a value read from the ADC. It should return a floating point value. This function must be present in the script for USER_ANALOG and USER_INDICATOR sensor types, otherwise the sensor will not be added to the schedule.

---

`void pre_process_sensor_value(string sensor_id)`

This function will be called right after ADC/digital input/create_sensor_value value acquisition. At this point the reading is not interpolated (if it's a Physical Analog sensor), and the formula is not applied.

---

`void post_process_sensor_value(string sensor_id)`

This function will be called after the sensor value is processed. At this point the reading is interpolated (if it's a Physical Analog sensor), and the formula is applied.

### Examples

#### USER_ANALOG sensor script

Note this sensor type generates initial value in the create_sensor_value function.

```lua
-- State variable (persisted across calls)
local counter = 0

function create_sensor_value(sensor_id)
    return 0.0
end

function pre_process_sensor_value(sensor_id)
    local value = get_sensor_value(sensor_id)
    update_sensor_value(sensor_id, value + counter)
    counter = counter + 1
end

function post_process_sensor_value(sensor_id)
    local value = get_sensor_value(sensor_id)
    update_sensor_value(sensor_id, value + counter * 4)
end
```

#### PHYSICAL_ANALOG sensor script

```lua
-- State variable (persisted across calls)
local counter = 0

function pre_process_sensor_value(sensor_id)
    local value = get_sensor_value(sensor_id)
    update_sensor_value(sensor_id, value + counter)
    counter = counter + 1
end

function post_process_sensor_value(sensor_id)
    local value = get_sensor_value(sensor_id)
    update_sensor_value(sensor_id, value + counter * 4)
end
```

## CAN Message API

### Functions user can define

`string process_can_frame(int frame_id, string data)`

This function will be called after a CAN frame is created and values are assigned. The frame must be defined in the DBC file. If the defined frame has no signals/values, data will be an empty string. The size of the string is equal to the size of the frame in bytes as defined in the DBC file. The type of data is string, but the underlying type is a byte array—each character in the string represents a byte.

This function must return a string of the same size as the frame in bytes.

### Examples

#### CAN Message script

This example generates a CAN frame with a gear value and a checksum, iterating through the gears every 1000 counts.

```lua
-- State variables (persisted across calls)
local counter = 0
local alive_counter = 0
local current_gear = 0

-- Gear values mapping
local GEARS = {
    [0] = 0,   -- Clear Screen
    [1] = 1,   -- 1st Gear
    [2] = 2,   -- 2nd Gear
    [3] = 3,   -- 3rd Gear
    [4] = 4,   -- 4th Gear
    [5] = 5,   -- Auto/D
    [6] = 6,   -- Neutral
    [7] = 7,   -- Reverse
    [8] = 9,   -- 5th Gear
    [9] = 10   -- 6th Gear
}

-- Calculate checksum according to the provided logic
local function calculateChecksum(gear_info, alive_cnt)
    local tmp1 = alive_cnt ~ gear_info  -- XOR
    local tmp2 = ~tmp1                  -- NOT
    local tmp3 = tmp2 & 0x0f            -- Mask lower 4 bits
    local tmp4 = tmp3 << 4              -- Shift to upper nibble
    local tmp5 = tmp4 | alive_cnt       -- OR with alive counter
    
    return tmp5 & 0xFF                  -- Ensure it's a byte
end

-- Main processing function
function process_can_frame(frame_id, data)
    -- Increment counter and cycle gear every 5000 counts
    counter = counter + 1
    if counter >= 1000 then
        counter = 0
        current_gear = (current_gear + 1) % 10  -- Cycle through 0-9
    end
    
    -- Get the actual gear value
    local gear_value = GEARS[current_gear]
    
	-- Create 8-byte frame (initialize with zeros)
	local bytes = {0, 0, 0, 0, 0, 0, 0, 0}

	-- Byte 1: bits 0-3 = selected gear
	bytes[2] = gear_value & 0x0F

	-- Byte 3: Calculate checksum with alive counter
	bytes[4] = calculateChecksum(gear_value, alive_counter)

	-- Increment alive counter (0-15, wraps around)
	alive_counter = (alive_counter + 1) % 16
    
    -- Convert bytes table to string
    return string.char(table.unpack(bytes))
end
```
