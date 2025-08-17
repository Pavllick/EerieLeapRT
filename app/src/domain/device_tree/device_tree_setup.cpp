#include <filesystem>

#include <zephyr/logging/log.h>

#include "device_tree_setup.h"

namespace eerie_leap::domain::device_tree {

LOG_MODULE_REGISTER(dt_setup_logger);

std::optional<fs_mount_t> DeviceTreeSetup::int_fs_mp_ = std::nullopt;
std::optional<fs_mount_t> DeviceTreeSetup::sd_fs_mp_ = std::nullopt;
std::optional<char*> DeviceTreeSetup::modbus_iface_ = std::nullopt;
std::optional<std::vector<gpio_dt_spec>> DeviceTreeSetup::gpio_specs_ = std::nullopt;
std::optional<std::vector<AdcDTInfo>> DeviceTreeSetup::adc_infos_ = std::nullopt;

void DeviceTreeSetup::Initialize() {
    InitInternalFs();
    InitSdFs();
    InitModbus();
    InitGpio();
    InitAdc();
}

DeviceTreeSetup& DeviceTreeSetup::Get() {
    static DeviceTreeSetup instance;
    return instance;
}

void DeviceTreeSetup::InitInternalFs() {
    int_fs_mp_ = std::make_optional<fs_mount_t>(FS_FSTAB_ENTRY(INT_FS_NODE));
    LOG_INF("Internal File System initialized.");
}

void DeviceTreeSetup::InitSdFs() {
#if DT_HAS_ALIAS(sdhc0)
    static std::string disk_name = CONFIG_EERIE_LEAP_SD_DRIVE_NAME;

    static std::string mount_point = CONFIG_EERIE_LEAP_SD_MOUNT_POINT;
    std::filesystem::path full_path(mount_point);
    mount_point = full_path.string();

    sd_fs_mp_ = {
        .type = FS_LITTLEFS,
        .mnt_point = mount_point.c_str(),
        .storage_dev = (void *)disk_name.c_str(),
        .flags = FS_MOUNT_FLAG_USE_DISK_ACCESS,
    };
    LOG_INF("SD File System initialized.");
#endif
}

void DeviceTreeSetup::InitModbus() {
#if DT_HAS_ALIAS(modbus0)
    modbus_iface_ = std::make_optional<char*>(DEVICE_DT_NAME(MODBUS_NODE));
    LOG_INF("Modbus initialized.");
#endif
}

void DeviceTreeSetup::InitGpio() {
#if DT_HAS_ALIAS(gpioc)
    gpio_specs_ = { DT_FOREACH_CHILD_SEP(GPIOC0_NODE, GPIO_SPEC, (,)) };
    LOG_INF("GPIO initialized.");
#endif
}

void DeviceTreeSetup::InitAdc() {
#if DT_HAS_ALIAS(adc0) || DT_HAS_ALIAS(adc1) || DT_HAS_ALIAS(adc2) || DT_HAS_ALIAS(adc3)
    adc_infos_ = std::make_optional<std::vector<AdcDTInfo>>();
#endif

#if DT_HAS_ALIAS(adc0)
    AdcDTInfo adc_info0 = {
        .adc_device = DEVICE_DT_GET(ADC_NODE(0)),
        .channel_configs = {DT_FOREACH_CHILD_SEP(ADC_NODE(0), ADC_CHANNEL_CFG_DT, (,))},
        .references_mv = {DT_FOREACH_CHILD_SEP(ADC_NODE(0), ADC_CHANNEL_VREF, (,))},
        .resolutions = {DT_FOREACH_CHILD_SEP(ADC_NODE(0), ADC_CHANNEL_RESOLUTION, (,))}
    };
    adc_infos_.value().push_back(adc_info0);

    LOG_INF("ADC0 initialized.");
#endif

#if DT_HAS_ALIAS(adc1)
    AdcDTInfo adc_info1 = {
        .adc_device = DEVICE_DT_GET(ADC_NODE(1)),
        .channel_configs = {DT_FOREACH_CHILD_SEP(ADC_NODE(1), ADC_CHANNEL_CFG_DT, (,))},
        .references_mv = {DT_FOREACH_CHILD_SEP(ADC_NODE(1), ADC_CHANNEL_VREF, (,))},
        .resolutions = {DT_FOREACH_CHILD_SEP(ADC_NODE(1), ADC_CHANNEL_RESOLUTION, (,))}
    };
    adc_infos_.value().push_back(adc_info1);

    LOG_INF("ADC1 initialized.");
#endif

#if DT_HAS_ALIAS(adc2)
    AdcDTInfo adc_info2 = {
        .adc_device = DEVICE_DT_GET(ADC_NODE(2)),
        .channel_configs = {DT_FOREACH_CHILD_SEP(ADC_NODE(2), ADC_CHANNEL_CFG_DT, (,))},
        .references_mv = {DT_FOREACH_CHILD_SEP(ADC_NODE(2), ADC_CHANNEL_VREF, (,))},
        .resolutions = {DT_FOREACH_CHILD_SEP(ADC_NODE(2), ADC_CHANNEL_RESOLUTION, (,))}
    };
    adc_infos_.value().push_back(adc_info2);

    LOG_INF("ADC2 initialized.");
#endif

#if DT_HAS_ALIAS(adc3)
    AdcDTInfo adc_info3 = {
        .adc_device = DEVICE_DT_GET(ADC_NODE(3)),
        .channel_configs = {DT_FOREACH_CHILD_SEP(ADC_NODE(3), ADC_CHANNEL_CFG_DT, (,))},
        .references_mv = {DT_FOREACH_CHILD_SEP(ADC_NODE(3), ADC_CHANNEL_VREF, (,))},
        .resolutions = {DT_FOREACH_CHILD_SEP(ADC_NODE(3), ADC_CHANNEL_RESOLUTION, (,))}
    };
    adc_infos_.value().push_back(adc_info3);

    LOG_INF("ADC3 initialized.");
#endif
}

} // namespace eerie_leap::domain::device_tree
