#include <zephyr/logging/log.h>

#include "modbus.h"

LOG_MODULE_REGISTER(modbus_logger);

namespace eerie_leap::subsys::modbus {

const uint8_t Modbus::SERVER_ID_ALL;

Modbus::Modbus(const char* iface_name) : iface_name_(iface_name) {
    client_iface_ = modbus_iface_get_by_name(iface_name_);

    client_params_ = {
        .mode = MODBUS_MODE_RTU,
        .rx_timeout = CONFIG_EERIE_LEAP_MODBUS_RX_TIMEOUT,
        .serial = {
            .baud = CONFIG_EERIE_LEAP_MODBUS_BAUD_RATE,
            .parity = UART_CFG_PARITY_NONE,
        },
    };
}

int Modbus::Initialize() {
    return modbus_init_client(client_iface_, client_params_);
};

int Modbus::ReadCoils(uint8_t server_id, uint16_t address, const void* data, size_t size_bytes) {
    return modbus_read_coils(client_iface_, server_id, address, (uint8_t*)data, size_bytes);
}

int Modbus::ReadDiscreteInputs(uint8_t server_id, uint16_t address, const void* data, size_t size_bytes) {
    return modbus_read_dinputs(client_iface_, server_id, address, (uint8_t*)data, size_bytes);
}

int Modbus::ReadHoldingRegisters(uint8_t server_id, uint16_t address, const void* data, size_t size_bytes) {
    if(size_bytes % 2 != 0) {
        LOG_ERR("Modbus read holding registers: size_bytes must be even.");
        return -1;
    }

    return modbus_read_holding_regs(client_iface_, server_id, address, (uint16_t*)data, size_bytes / 2);
}

int Modbus::ReadInputRegisters(uint8_t server_id, uint16_t address, const void* data, size_t size_bytes) {
    if(size_bytes % 2 != 0) {
        LOG_ERR("Modbus read input registers: size_bytes must be even.");
        return -1;
    }

    int res = modbus_read_input_regs(client_iface_, server_id, address, (uint16_t*)data, size_bytes / 2);

    if(res < 0) {
        LOG_ERR("Modbus read input registers: failed to read input registers.");
    }

    return res;
}

int Modbus::WriteCoil(uint8_t server_id, uint16_t address, const void* data, const bool value) {
    return modbus_write_coil(client_iface_, server_id, address, value);
}

int Modbus::WriteHoldingRegister(uint8_t server_id, uint16_t address, const uint16_t data) {
    return modbus_write_holding_reg(client_iface_, server_id, address, data);
}

int Modbus::WriteCoils(uint8_t server_id, uint16_t address, const void* data, size_t size_bytes) {
    return modbus_write_coils(client_iface_, server_id, address, (uint8_t*)data, size_bytes);
}

int Modbus::WriteHoldingRegisters(uint8_t server_id, uint16_t address, const void* data, size_t size_bytes) {
    if(size_bytes % 2 != 0) {
        LOG_ERR("Modbus write registers: size_bytes must be even.");
        return -1;
    }

    return modbus_write_holding_regs(client_iface_, server_id, address, (uint16_t*)data, size_bytes / 2);
}

}  // namespace eerie_leap::subsys::modbus
