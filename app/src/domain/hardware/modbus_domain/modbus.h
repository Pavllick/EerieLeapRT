#pragma once

#include <zephyr/modbus/modbus.h>

namespace eerie_leap::domain::hardware::modbus_domain {

class Modbus {
private:
    const char* iface_name_;
    modbus_iface_param client_params_;
    int client_iface_;

public:
    Modbus(const char* iface_name);

    int Initialize();
    int ReadRegisters(uint8_t address, uint16_t start_address, void* data, size_t count);
    int WriteRegisters(uint8_t address, uint16_t start_address, void* data, size_t count);
};

}  // namespace eerie_leap::domain::hardware::modbus_domain
