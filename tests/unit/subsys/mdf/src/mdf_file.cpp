#include <cstdint>
#include <memory>
#include <streambuf>

#include <zephyr/ztest.h>
#include "zephyr/kernel.h"

#include "utilities/time/rtc_service.h"
#include "subsys/device_tree/dt_fs.h"
#include "subsys/fs/services/fs_service.h"
#include "subsys/fs/services/fs_service_stream_buf.h"
#include "subsys/mdf/mdf4_file.h"

using namespace eerie_leap::utilities::time;
using namespace eerie_leap::subsys::device_tree;
using namespace eerie_leap::subsys::fs::services;
using namespace eerie_leap::subsys::mdf;

ZTEST_SUITE(mdf_file, NULL, NULL, NULL, NULL, NULL);

ZTEST(mdf_file, test_WriteToStream) {
    RtcService rtc_service;

    // DtFs::InitInternalFs();
    // auto fs_service = std::make_shared<FsService>(DtFs::GetInternalFsMp().value());
    // zassert_true(fs_service->Initialize());

    k_msleep(100);

    Mdf4File mdf_file(false);
    mdf_file.UpdateCurrentTime(rtc_service.GetCurrentTime());
    auto data_group = mdf_file.CreateDataGroup();
    auto channel_group = mdf_file.CreateChannelGroup(*data_group);
    mdf_file.CreateDataChannel(*channel_group, mdf4::ChannelBlock::DataType::FloatLe, "engine_speed", "rpm");
    mdf_file.CreateDataChannel(*channel_group, mdf4::ChannelBlock::DataType::FloatLe, "pressure", "bar");


    // FsServiceStreamBuf fs_buf(fs_service.get(), "output/data.mf4");
    // auto bytes_written = mdf_file.WriteToStream(fs_buf);
    // fs_buf.close();

    std::ofstream file("../../../../../../../../../test_file.mf4", std::ios::binary);
    mdf_file.WriteToStream(*file.rdbuf());
    file.close();

    // zassert_not_equal(bytes_written, 0);
}
