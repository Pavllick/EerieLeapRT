#pragma once

#include <memory>
#include <vector>

#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "i_adc.h"
#include "i_adc_manager.h"
#include "domain/hardware/adc_domain/models/adc_configuration.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::domain::hardware::adc_domain::models;

class AdcSimulator : public IAdc {
private:
    uint16_t samples_ = 0;
    std::shared_ptr<AdcChannelConfiguration> adc_channel_configuration_ = nullptr;

public:
    AdcSimulator() = default;
    ~AdcSimulator() = default;

    int Initialize() override;
    void UpdateConfiguration(uint16_t samples) override;
    float ReadChannel(int channel) override;
    int GetChannelCount() override;
};

class AdcSimulatorManager : public IAdcManager {
    private:
        std::shared_ptr<std::vector<std::shared_ptr<AdcChannelConfiguration>>> adc_channel_configuration_;
        std::shared_ptr<IAdc> adc_;

    public:
        AdcSimulatorManager() {
            adc_channel_configuration_ = nullptr;
            adc_ = std::make_shared<AdcSimulator>();
        }

        int Initialize() override {
            return adc_->Initialize();
        }

        void UpdateConfiguration(std::shared_ptr<AdcConfiguration>& adc_configuration) override {
            adc_->UpdateConfiguration(adc_configuration->samples);
        }

        std::shared_ptr<AdcChannelConfiguration> GetChannelConfiguration(int channel) override {
            return adc_channel_configuration_->at(channel);
        }

        float ReadChannel(int channel) override {
            return adc_->ReadChannel(channel);
        }

        int GetAdcCount() override {
            return 1;
        }

        int GetChannelCount() override {
            return adc_->GetChannelCount();
        }
    };

}  // namespace eerie_leap::domain::hardware::adc_domain
