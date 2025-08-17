#pragma once

#include <vector>
#include <optional>

#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/fs/fs.h>

#include "adc_dt_info.h"

// File System
#define INT_FS_NODE DT_ALIAS(lfs1)
FS_FSTAB_DECLARE_ENTRY(INT_FS_NODE);

// Modbus
#if DT_HAS_ALIAS(modbus0)
#define MODBUS_NODE DT_ALIAS(modbus0)
#endif

// GPIO
#if DT_HAS_ALIAS(gpioc)
#define GPIOC0_NODE DT_ALIAS(gpioc)
#define GPIO_SPEC(node_id) GPIO_DT_SPEC_GET(node_id, gpios)
#endif

// ADC
#define ADC_NODE(idx) DT_ALIAS(adc ## idx)
#define ADC_CHANNEL_VREF(node_id) DT_PROP_OR(node_id, zephyr_vref_mv, 0)
#define ADC_CHANNEL_RESOLUTION(node_id) DT_PROP_OR(node_id, zephyr_resolution, 0)

namespace eerie_leap::domain::device_tree {

class DeviceTreeSetup {
private:
    static std::optional<fs_mount_t> int_fs_mp_;
    static std::optional<fs_mount_t> sd_fs_mp_;
    static std::optional<char*> modbus_iface_;
    static std::optional<std::vector<gpio_dt_spec>> gpio_specs_;
    static std::optional<std::vector<AdcDTInfo>> adc_infos_;

    DeviceTreeSetup() = default;

    static void InitInternalFs();
    static void InitSdFs();
    static void InitModbus();
    static void InitGpio();
    static void InitAdc();

public:
    static DeviceTreeSetup& Get();
    static void Initialize();

    static std::optional<fs_mount_t>& GetInternalFsMp() { return int_fs_mp_; }
    static std::optional<fs_mount_t>& GetSdFsMp() { return sd_fs_mp_; }
    static std::optional<char*>& GetModbusIface() { return modbus_iface_; }
    static std::optional<std::vector<gpio_dt_spec>>& GetGpioSpecs() { return gpio_specs_; }
    static std::optional<std::vector<AdcDTInfo>>& GetAdcInfos() { return adc_infos_; }
};

} // namespace eerie_leap::domain::device_tree
