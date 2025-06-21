#pragma once

#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/random/random.h>

#include "i_adc_manager.h"
#include "domain/hardware/adc_domain/models/adc_configuration.h"

namespace eerie_leap::domain::hardware::adc_domain {

using namespace eerie_leap::domain::hardware::adc_domain::models;

class AdcSimulator : public IAdc {
private:
    std::shared_ptr<AdcConfiguration> adc_configuration_;

public:
    AdcSimulator() = default;
    ~AdcSimulator() = default;

    int Initialize() override;
    void UpdateConfiguration(std::shared_ptr<AdcConfiguration> adc_configuration) override;
    std::shared_ptr<AdcConfiguration> GetConfiguration() override;
    float ReadChannel(int channel) override;
    int GetChannelCount() override;
};

class AdcSimulatorManager : public IAdcManager {
    private:
        std::shared_ptr<IAdc> adc_;

    public:
        AdcSimulatorManager() {
            adc_ = std::make_shared<AdcSimulator>();
        }

        int Initialize() override {
            return adc_->Initialize();
        }

        void UpdateConfigurations(std::shared_ptr<std::vector<std::shared_ptr<AdcConfiguration>>>& adc_configurations) override {
            adc_->UpdateConfiguration(adc_configurations->at(0));
        }

        std::shared_ptr<IAdc> GetAdcForChannel(int channel) override {
            return adc_;
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
