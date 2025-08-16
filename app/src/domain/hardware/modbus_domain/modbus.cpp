#include <zephyr/logging/log.h>

#include "modbus.h"

LOG_MODULE_REGISTER(modbus_logger);

namespace eerie_leap::domain::hardware::modbus_domain {

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

int Modbus::ReadRegisters(uint8_t address, uint16_t start_address, void* data, size_t count) {
    return modbus_read_holding_regs(client_iface_, address, start_address, (uint16_t*)data, count);
}

int Modbus::WriteRegisters(uint8_t address, uint16_t start_address, void* data, size_t count) {
    return modbus_write_holding_regs(client_iface_, address, start_address, (uint16_t*)data, count);
}

}  // namespace eerie_leap::domain::hardware::modbus_domain
