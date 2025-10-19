#pragma once

#include <cstdint>

#include "subsys/mdf/mdf4/channel_block.h"
#include "subsys/mdf/mdf_data_type.h"

namespace eerie_leap::subsys::mdf {

class MdfHelpers {
public:
    struct ChannelDataType {
        mdf4::ChannelBlock::DataType data_type;
        uint32_t bit_count;
    };

    static ChannelDataType ToMdf4ChannelDataType(MdfDataType data_type);
};

} // namespace eerie_leap::subsys::mdf
