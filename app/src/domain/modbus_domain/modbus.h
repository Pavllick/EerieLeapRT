#pragma once

#include <zephyr/modbus/modbus.h>

namespace eerie_leap::domain::modbus_domain {

class Modbus {
private:
    const char* iface_name_;
    modbus_iface_param client_params_;
    int client_iface_;

public:
    Modbus(const char* iface_name);

    int Initialize();
    int ReadCoils(uint8_t server_id, uint16_t start_address, void* data, size_t size_bytes);
    int ReadDiscreteInputs(uint8_t server_id, uint16_t start_address, void* data, size_t size_bytes);
    int ReadHoldingRegisters(uint8_t server_id, uint16_t start_address, void* data, size_t size_bytes);
    int ReadInputRegisters(uint8_t server_id, uint16_t start_address, void* data, size_t size_bytes);
    int WriteCoil(uint8_t server_id, uint16_t start_address, void* data, const bool value);
    int WriteHoldingRegister(uint8_t server_id, uint16_t start_address, uint16_t data);
    int WriteCoils(uint8_t server_id, uint16_t start_address, void* data, size_t size_bytes);
    int WriteHoldingRegisters(uint8_t server_id, uint16_t start_address, void* data, size_t size_bytes);
};

}  // namespace eerie_leap::domain::modbus_domain
