#include <zephyr/logging/log.h>

#include "dt_adc.h"

namespace eerie_leap::domain::device_tree {

LOG_MODULE_REGISTER(dt_adc_logger);

std::optional<std::vector<AdcDTInfo>> DtAdc::adc_infos_ = std::nullopt;

void DtAdc::Initialize() {
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
