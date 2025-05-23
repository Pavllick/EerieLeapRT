#include <stdexcept>
#include <numeric>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/adc/adc_emul.h>
#include <zephyr/kernel.h>

#include "adc.h"

namespace eerie_leap::domain::adc_domain::hardware {

LOG_MODULE_REGISTER(adc_logger);

int Adc::Initialize() {
    LOG_INF("Adc initialization started.");

	adc_device_ = DEVICE_DT_GET(ADC_NODE);

	if(!device_is_ready(adc_device_)) {
		LOG_ERR("ADC device not ready");
		return -1;
	}

    if(dt_adc_channel_count_ == 0) {
        LOG_ERR("No ADC channels configured in device tree");
        return -1;
    }

    LOG_INF("ADC initialized with %d channels", dt_adc_channel_count_);

	/* Configure channels. */
	for(size_t i = 0U; i < dt_adc_channel_count_; i++) {
		int err = adc_channel_setup(adc_device_, &channel_configs_[i]);
		if(err < 0) {
			LOG_ERR("Could not setup channel #%d (%d)\n", i, err);
			return 0;
		}

        if(channel_configs_[i].reference == ADC_REF_INTERNAL)
		    references_mv_[i] = adc_ref_internal(adc_device_);

        available_channels_.insert(channel_configs_[i].channel_id);

		LOG_INF("Channel %d configured", channel_configs_[i].channel_id);
	}

    LOG_INF("Adc initialized successfully.");

    return 0;
}

void Adc::UpdateConfiguration(AdcConfiguration config) {
    adc_config_ = config;

	samples_buffer_ = std::make_unique<uint16_t[]>(adc_config_->samples);

	sequence_options_ = {
		.interval_us = 0,
		.extra_samplings = static_cast<uint16_t>(adc_config_->samples - 1),
	};

	for(size_t i = 0U; i < dt_adc_channel_count_; i++) {
		sequences_[i] = {
			.options     = &sequence_options_,
			.buffer      = samples_buffer_.get(),
			.buffer_size = sizeof(uint16_t) * adc_config_->samples,
			.resolution  = resolutions_[i],
		};
	}

    LOG_INF("Adc configuration updated.");
}

double Adc::ReadChannel(int channel) {
    if(!adc_config_)
        throw std::runtime_error("ADC config is not set!");

    if(available_channels_.find(channel) == available_channels_.end())
        throw std::invalid_argument("ADC channel is not available.");

	sequences_[channel].channels = BIT(channel_configs_[channel].channel_id);

    int err = adc_read(adc_device_, &sequences_[channel]);
    if(err < 0) {
        LOG_ERR("Can not read channel: %d, error: (%d)\n", channel, err);
		return 0;
    }

	int32_t val_mv = GetReding();
	err = adc_raw_to_millivolts(references_mv_[channel], channel_configs_[channel].gain, sequences_[channel].resolution, &val_mv);

	if((err < 0) || references_mv_[channel] == 0) {
		LOG_ERR("ADC conversion for channel %d to mV failed (%d)\n", channel, err);
		return 0;
	}

    return (double)val_mv / 1000.0;
}

uint16_t Adc::GetReding() {
	uint16_t value = std::accumulate(samples_buffer_.get(), samples_buffer_.get() + adc_config_->samples, 0, std::plus<int>());

	return value / adc_config_->samples;
}

}  // namespace eerie_leap::domain::adc_domain::hardware
