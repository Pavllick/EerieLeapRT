#include <vector>
#include <zephyr/sys/util.h>
#include <zephyr/kernel.h>

#include "subsys/time/time_helpers.hpp"
#include "utilities/memory/heap_allocator.h"
#include "subsys/adc/utilities/adc_calibrator.h"
#include "domain/sensor_domain/utilities/sensor_readings_frame.hpp"
#include "domain/sensor_domain/sensor_readers/sensor_reader_physical_analog_calibrator.h"
#include "calibration_service.h"

namespace eerie_leap::domain::sensor_domain::services {

using namespace eerie_leap::utilities::memory;
using namespace eerie_leap::subsys::adc::utilities;
using namespace eerie_leap::domain::sensor_domain::models;
using namespace eerie_leap::domain::sensor_domain::sensor_readers;

LOG_MODULE_REGISTER(calibration_logger);

CalibrationService::CalibrationService(
    std::shared_ptr<ITimeService> time_service,
    std::shared_ptr<GuidGenerator> guid_generator,
    std::shared_ptr<AdcConfigurationManager> adc_configuration_manager,
    std::shared_ptr<ProcessingSchedulerService> processing_scheduler_service)
    : time_service_(std::move(time_service)),
    guid_generator_(std::move(guid_generator)),
    adc_configuration_manager_(std::move(adc_configuration_manager)),
    processing_scheduler_service_(std::move(processing_scheduler_service)) {

    k_sem_init(&processing_semaphore_, 1, 1);
};

void CalibrationService::ProcessCalibrationWorkTask(k_work* work) {
    SensorTask* task = CONTAINER_OF(work, SensorTask, work);

    if(k_sem_take(task->processing_semaphore, PROCESSING_TIMEOUT) == 0) {
        try {
            task->reader->Read();
            auto reading = task->readings_frame->GetReading(task->sensor->id);

            printk("ADC Calibration Reading: Value: %.3f, Time: %s\n",
                reading->value.value_or(0.0f),
                TimeHelpers::GetFormattedString(*reading->timestamp).c_str());
        } catch (const std::exception& e) {
            LOG_ERR("Error processing calibrator on channel %d, Error: %s",
                task->sensor->configuration.channel.value_or(-1),
                e.what());
        }
    } else {
        LOG_ERR("Lock take timed out for calibrator on channel %d",
            task->sensor->configuration.channel.value_or(-1));
    }

    k_sem_give(task->processing_semaphore);
    k_work_reschedule(&task->work, task->sampling_rate_ms);
}

std::shared_ptr<SensorTask> CalibrationService::CreateCalibrationTask(int channel) {
    std::shared_ptr<Sensor> sensor = make_shared_ext<Sensor>();
    sensor->id = "CalibrationSensor";
    sensor->configuration.type = SensorType::PHYSICAL_ANALOG;
    sensor->configuration.channel = channel;
    sensor->configuration.sampling_rate_ms = CONFIG_EERIE_LEAP_ADC_CALIBRATION_SAMPLING_RATE_MS;

    auto sensor_readings_frame = make_shared_ext<SensorReadingsFrame>();

    auto task = make_shared_ext<SensorTask>();
    task->processing_semaphore = &processing_semaphore_;
    task->sampling_rate_ms = K_MSEC(sensor->configuration.sampling_rate_ms);
    task->sensor = sensor;
    task->readings_frame = sensor_readings_frame;

    task->reader = std::make_unique<SensorReaderPhysicalAnalogCalibrator>(
        time_service_,
        guid_generator_,
        sensor_readings_frame,
        sensor,
        adc_configuration_manager_);

    return task;
}

void CalibrationService::Start(int channel) {
    processing_scheduler_service_->Pause();

    auto adc_manager = adc_configuration_manager_->Get();
    adc_manager->UpdateSamplesCount(CONFIG_EERIE_LEAP_ADC_CALIBRATION_SAMPLES_COUNT);

    calibration_task_ = CreateCalibrationTask(channel);

    k_work_delayable* work = &calibration_task_->work;
    k_work_init_delayable(work, ProcessCalibrationWorkTask);

    k_work_schedule(work, K_NO_WAIT);

    k_sleep(K_MSEC(1));

    LOG_INF("Calibration Service started");
}

void CalibrationService::Stop() {
    k_work_sync sync;

    while(calibration_task_ != nullptr) {
        bool res = k_work_cancel_delayable_sync(&calibration_task_->work, &sync);
        if(!res) {
            calibration_task_.reset();
            break;
        }
    }

    LOG_INF("Calibration Service stopped");

    auto adc_manager = adc_configuration_manager_->Get();
    adc_manager->ResetSamplesCount();

    processing_scheduler_service_->Resume();
}

} // namespace eerie_leap::domain::sensor_domain::services
