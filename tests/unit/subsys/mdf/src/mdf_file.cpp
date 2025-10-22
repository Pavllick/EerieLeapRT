#include <cstdint>
#include <memory>
#include <streambuf>
#include <vector>

#include <zephyr/ztest.h>
#include "zephyr/kernel.h"

#include "subsys/time/rtc_provider.h"
#include "subsys/device_tree/dt_fs.h"
#include "subsys/fs/services/fs_service.h"
#include "subsys/fs/services/fs_service_stream_buf.h"
#include "subsys/mdf/mdf4/channel_group_block.h"
#include "subsys/mdf/mdf4/source_information_block.h"
#include "subsys/mdf/mdf4/channel_conversion_block.h"
#include "subsys/mdf/mdf_data_type.h"
#include "subsys/mdf/mdf4_file.h"

using namespace eerie_leap::subsys::time;
using namespace eerie_leap::subsys::device_tree;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::mdf;

ZTEST_SUITE(mdf_file, NULL, NULL, NULL, NULL, NULL);

ZTEST(mdf_file, test_WriteToStream) {
    RtcProvider rtc_time_provider;

    // DtFs::InitInternalFs();
    // auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());
    // zassert_true(fs_service->Initialize());

    Mdf4File mdf_file(false);
    mdf_file.UpdateCurrentTime(rtc_time_provider.GetTime());
    auto data_group = mdf_file.CreateDataGroup(1);
    auto channel_group = mdf_file.CreateChannelGroup(*data_group, 1, "pressure");
    auto source_information = std::make_shared<mdf4::SourceInformationBlock>(
        mdf4::SourceInformationBlock::SourceType::IoDevice,
        mdf4::SourceInformationBlock::BusType::None,
        "Eerie Leap Sensor");
    channel_group->AddSourceInformation(source_information);

    // mdf_file.CreateDataChannel(*channel_group, MdfDataType::Float32, "engine_speed", "rpm");
    auto channel = mdf_file.CreateDataChannel(*channel_group, MdfDataType::Uint64, "value", "bar");
    auto conversion = mdf4::ChannelConversionBlock::CreateAlgebraicConversion("x * 0.1");
    channel->SetConversion(std::make_shared<mdf4::ChannelConversionBlock>(conversion));

    // FsServiceStreamBuf fs_buf(fs_service.get(), "output/data.mf4");
    // auto bytes_written = mdf_file.WriteFileToStream(fs_buf);
    // fs_buf.close();

    // Will create file in the twister-out directory
    std::ofstream file("../../../../../../../../test_file.mf4", std::ios::binary);
    mdf_file.WriteFileToStream(*file.rdbuf());

    auto record = mdf_file.CreateDataRecord(channel_group);
    for(uint64_t i = 0; i < 200; i++) {
        float time = i * 0.1;
        uint64_t pressure = i * i;
        record.WriteToStream(*file.rdbuf(), {&time, &pressure});
    }

    file.close();

    // zassert_not_equal(bytes_written, 0);
}
