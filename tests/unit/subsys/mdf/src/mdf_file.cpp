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
#include "subsys/mdf/mdf4/vlsd_data_record.h"
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

    auto channel_group_1 = mdf_file.CreateChannelGroup(data_group, 100, "pressure");
    auto source_information_1 = std::make_shared<mdf4::SourceInformationBlock>(
        mdf4::SourceInformationBlock::SourceType::IoDevice,
        mdf4::SourceInformationBlock::BusType::None);
    source_information_1->SetName(mdf_file.GetOrCreateTextBlock("Eerie Leap Sensor"));
    channel_group_1->AddSourceInformation(source_information_1);

    // mdf_file.CreateDataChannel(*channel_group, MdfDataType::Float32, "engine_speed", "rpm");
    auto channel_1 = mdf_file.CreateDataChannel(channel_group_1, MdfDataType::Uint64, "value", "bar");
    channel_1->SetName(mdf_file.GetOrCreateTextBlock("value"));
    channel_1->SetUnit(mdf_file.GetOrCreateTextBlock("bar"));
    auto conversion_1 = mdf4::ChannelConversionBlock::CreateAlgebraicConversion("x * 0.1");
    channel_1->SetConversion(std::make_shared<mdf4::ChannelConversionBlock>(conversion_1));

    auto channel_group_2 = mdf_file.CreateChannelGroup(data_group, 101, "pressure");
    channel_group_2->AddSourceInformation(source_information_1);

    // mdf_file.CreateDataChannel(*channel_group, MdfDataType::Float32, "engine_speed", "rpm");
    auto channel_2 = mdf_file.CreateDataChannel(channel_group_2, MdfDataType::Uint64, "value", "bar");
    auto conversion_2 = mdf4::ChannelConversionBlock::CreateAlgebraicConversion("x * 0.1");
    channel_2->SetConversion(std::make_shared<mdf4::ChannelConversionBlock>(conversion_2));

    auto vlsd_channel_group_3 = mdf_file.CreateVLSDChannelGroup(data_group, 110);
    auto channel_group_3 = mdf_file.CreateCanDataFrameChannelGroup(data_group, vlsd_channel_group_3, 102, "Raw CAN Frame");

    auto vlsd_channel_group_4 = mdf_file.CreateVLSDChannelGroup(data_group, 111);
    auto channel_group_4 = mdf_file.CreateCanDataFrameChannelGroup(data_group, vlsd_channel_group_4, 103, "Raw CAN Frame");

    // FsServiceStreamBuf fs_buf(fs_service.get(), "output/data.mf4");
    // auto bytes_written = mdf_file.WriteFileToStream(fs_buf);
    // fs_buf.close();

    // Will create file in the twister-out directory
    std::ofstream file("../../../../../../../../test_file.mf4", std::ios::binary);
    mdf_file.WriteFileToStream(*file.rdbuf());

    auto record_1 = mdf_file.CreateDataRecord(channel_group_1);
    for(uint64_t i = 0; i < 10; i++) {
        float time = i * 0.1;
        uint64_t pressure = i * i;
        record_1.WriteToStream(*file.rdbuf(), {&time, &pressure});
    }

    for(uint64_t i = 10; i < 20; i++) {
        float time = i * 0.1;

        CanFrame can_frame {
            .id = 0x123,
            .data = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0}
        };

        mdf_file.WriteCanbusDataRecordToStream(channel_group_3, *file.rdbuf(), can_frame, time);
    }

    auto record_1_2 = mdf_file.CreateDataRecord(channel_group_1);
    for(uint64_t i = 20; i < 30; i++) {
        float time = i * 0.1;
        uint64_t pressure = i * i;
        record_1_2.WriteToStream(*file.rdbuf(), {&time, &pressure});
    }


    constexpr size_t vlds_data_size_2 = 9;
    constexpr size_t vlds_block_size_2 = 1 + 4 + vlds_data_size_2;
    constexpr size_t record_size_2 = 4 + 10 + vlds_block_size_2;

    uint64_t vlds_offset_2 = 0;
    auto record_3_2 = mdf_file.CreateDataRecord(channel_group_4);
    for(uint64_t i = 30; i < 40; i++) {
        std::array<uint8_t, record_size_2> data;
        memset(data.data(), 0, data.size());

        int offset = 0;

        float time = i * 0.1;
        std::memcpy(data.data() + offset, &time, sizeof(float));
        offset += sizeof(float);

        uint32_t data_4_pack_0 = 0x00001FA1;
        // uint32_t frame_id = 0xA11F0000;
        // data_4_pack_0 = frame_id >> 2;
        std::memcpy(data.data() + offset, &data_4_pack_0, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        uint8_t data_1_pack_1 = 0;
        uint8_t frame_data_length = vlds_data_size_2;
        data_1_pack_1 = frame_data_length << 1;
        std::memcpy(data.data() + offset, &data_1_pack_1, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        uint8_t data_1_pack_2 = 0;
        data_1_pack_2 = frame_data_length << 2;
        std::memcpy(data.data() + offset, &data_1_pack_2, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        uint32_t data_4_pack_3 = vlds_offset_2;
        std::memcpy(data.data() + offset, &data_4_pack_3, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        // VLSD

        // Channel Group ID
        uint8_t data_1_pack_4 = vlsd_channel_group_4->GetRecordId();
        std::memcpy(data.data() + offset, &data_1_pack_4, sizeof(uint8_t));
        offset += sizeof(uint8_t);

        // VLSD Data Length
        uint32_t data_4_pack_5 = vlds_data_size_2;
        std::memcpy(data.data() + offset, &data_4_pack_5, sizeof(uint32_t));
        offset += sizeof(uint32_t);

        std::array<uint8_t, vlds_data_size_2> data_9_pack_6;
        memset(data_9_pack_6.data(), 0, data_9_pack_6.size());
        uint64_t vlds_data = 1234 + i * 10;
        std::memcpy(data_9_pack_6.data(), &vlds_data, sizeof(uint64_t));
        std::memcpy(data.data() + offset, data_9_pack_6.data(), vlds_data_size_2);
        offset += vlds_data_size_2;

        record_3_2.WriteToStream(*file.rdbuf(), data);

        vlds_offset_2 += 4 + vlds_data_size_2;
    }

    file.close();

    // zassert_not_equal(bytes_written, 0);
}
