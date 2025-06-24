#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "utilities/memory/heap_allocator.h"
#include "i_adc.h"
#include "i_adc_manager.h"
#include "domain/hardware/adc_domain/models/adc_configuration.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::utilities::memory;
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
            adc_ = make_shared_ext<AdcSimulator>();
            adc_configuration_ = nullptr;
        }

        int Initialize() override {
            return adc_->Initialize();
        }

        void UpdateConfiguration(std::shared_ptr<AdcConfiguration>& adc_configuration) override {
            adc_configuration_ = adc_configuration;

            adc_->UpdateConfiguration(adc_configuration->samples);
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
    };

}  // namespace eerie_leap::domain::hardware::adc_domain
