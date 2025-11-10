#pragma once

#if defined(CONFIG_ADC) || defined(CONFIG_ADC_EMUL)

#include <functional>
#include <memory>
#include <vector>

#include <zephyr/drivers/adc.h>

#include "subsys/device_tree/adc_dt_info.h"
#include "models/adc_configuration.h"

#include "i_adc_manager.h"
#include "i_adc.h"

namespace eerie_leap::subsys::adc {

using namespace eerie_leap::subsys::device_tree;
using namespace eerie_leap::subsys::adc::models;

class AdcManager : public IAdcManager {
private:
    std::vector<AdcDTInfo> adc_infos_;

protected:
    std::vector<std::shared_ptr<IAdc>> adcs_;
    std::shared_ptr<AdcConfiguration> adc_configuration_;

    bool IsChannelValid(int channel);

public:
    AdcManager(std::vector<AdcDTInfo> adc_infos);
    int Initialize() override;

    void UpdateConfiguration(std::shared_ptr<AdcConfiguration> adc_configuration) override;
    std::shared_ptr<AdcChannelConfiguration> GetChannelConfiguration(int channel) override;
    std::pair<IAdc*, int> GetAdcForChannel(int channel);
    std::function<float ()> GetChannelReader(int channel) override;
    int GetAdcCount() override;
    int GetChannelCount() override;
    void UpdateSamplesCount(int samples) override;
    void ResetSamplesCount() override;
};

}  // namespace eerie_leap::subsys::adc

#endif // defined(CONFIG_ADC) || defined(CONFIG_ADC_EMUL)
