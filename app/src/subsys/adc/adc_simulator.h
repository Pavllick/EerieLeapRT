#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "i_adc.h"
#include "i_adc_manager.h"
#include "models/adc_configuration.h"

namespace eerie_leap::subsys::adc {

using namespace eerie_leap::subsys::adc::models;

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
        std::shared_ptr<IAdc> adc_;
        std::shared_ptr<AdcConfiguration> adc_configuration_;

        bool IsChannelValid(int channel) {
            return adc_configuration_ != nullptr
                && channel >= 0
                && channel < adc_configuration_->channel_configurations->size()
                && channel < GetChannelCount();
        }

    public:
        AdcSimulatorManager() {
            adc_ = std::make_shared<AdcSimulator>();
            adc_configuration_ = nullptr;
        }

        int Initialize() override {
            return adc_->Initialize();
        }

        void UpdateConfiguration(std::shared_ptr<AdcConfiguration> adc_configuration) override {
            adc_configuration_ = std::move(adc_configuration);

            adc_->UpdateConfiguration(adc_configuration_->samples);
        }

        std::shared_ptr<AdcChannelConfiguration> GetChannelConfiguration(int channel) override {
            if(!IsChannelValid(channel))
                throw std::invalid_argument("ADC channel out of range.");

            return adc_configuration_->channel_configurations->at(channel);
        }

        std::function<float ()> GetChannelReader(int channel) override {
            if(!IsChannelValid(channel))
                throw std::invalid_argument("ADC channel out of range.");

            return [this, channel]() { return adc_->ReadChannel(channel); };
        }

        int GetAdcCount() override {
            return 1;
        }

        int GetChannelCount() override {
            return adc_->GetChannelCount();
        }

        void UpdateSamplesCount(int samples) override {
            adc_->UpdateConfiguration(samples);
        }

        void ResetSamplesCount() override {
            adc_->UpdateConfiguration(adc_configuration_->samples);
        }
    };

}  // namespace eerie_leap::subsys::adc
