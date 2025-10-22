#include "channel_conversion_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

ChannelConversionBlock::ChannelConversionBlock(ConversionType conversion_type)
    : BlockBase("CC") {

    conversion_type_ = conversion_type;
    precision_ = 0;
    flags_ = 0;
    reference_count_ = 0;
    value_count_ = 0;
    min_phisical_value_ = 0;
    max_phisical_value_ = 0;
    values_ = {};
}

ChannelConversionBlock ChannelConversionBlock::CreateAlgebraicConversion(const std::string& formula) {
    ChannelConversionBlock block(ConversionType::Algebraic);

    auto text_block = std::make_shared<TextBlock>();
    text_block->SetText(formula);
    block.links_.AddExtraLink(text_block);
    block.reference_count_ = 1;

    return block;
}

uint64_t ChannelConversionBlock::GetBlockSize() const {
    return GetBaseSize() + 1 + 1 + 2 + 2 + 2 + 8 + 8 + 8 * value_count_;
}

std::unique_ptr<uint8_t[]> ChannelConversionBlock::Serialize() const {
    auto buffer = SerializeBase();
    uint64_t offset = GetBaseSize();

    std::memcpy(buffer.get() + offset, &conversion_type_, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    std::memcpy(buffer.get() + offset, &precision_, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    std::memcpy(buffer.get() + offset, &flags_, sizeof(flags_));
    offset += sizeof(flags_);

    std::memcpy(buffer.get() + offset, &reference_count_, sizeof(reference_count_));
    offset += sizeof(reference_count_);

    std::memcpy(buffer.get() + offset, &value_count_, sizeof(value_count_));
    offset += sizeof(value_count_);

    std::memcpy(buffer.get() + offset, &min_phisical_value_, sizeof(min_phisical_value_));
    offset += sizeof(min_phisical_value_);

    std::memcpy(buffer.get() + offset, &max_phisical_value_, sizeof(max_phisical_value_));
    offset += sizeof(max_phisical_value_);

    for (size_t i = 0; i < value_count_; i++) {
        std::memcpy(buffer.get() + offset, &values_[i], sizeof(values_[i]));
        offset += sizeof(values_[i]);
    }

    return buffer;
}

} // namespace eerie_leap::subsys::mdf::mdf4
