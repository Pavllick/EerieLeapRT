#include "mdf_helpers.h"

namespace eerie_leap::subsys::mdf {

MdfHelpers::ChannelDataType MdfHelpers::ToMdf4ChannelDataType(MdfDataType data_type) {
    switch (data_type) {
        case MdfDataType::Int32:
            return { mdf4::ChannelBlock::DataType::SignedIntegerLe, sizeof(int32_t) * 8 };
        case MdfDataType::Int64:
            return { mdf4::ChannelBlock::DataType::SignedIntegerLe, sizeof(int64_t) * 8 };
        case MdfDataType::Uint32:
            return { mdf4::ChannelBlock::DataType::UnsignedIntegerLe, sizeof(uint32_t) * 8 };
        case MdfDataType::Uint64:
            return { mdf4::ChannelBlock::DataType::UnsignedIntegerLe, sizeof(uint64_t) * 8 };
        case MdfDataType::Float32:
            return { mdf4::ChannelBlock::DataType::FloatLe, sizeof(float) * 8 };
        case MdfDataType::Float64:
            return { mdf4::ChannelBlock::DataType::FloatLe, sizeof(double) * 8 };
        case MdfDataType::ByteArray:
            return { mdf4::ChannelBlock::DataType::ByteArray, 0 };
        default:
            throw std::runtime_error("Unsupported data type");
    }
}

} // namespace eerie_leap::subsys::mdf
