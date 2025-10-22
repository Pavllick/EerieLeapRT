#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "subsys/mdf/utilities/block_links.h"
#include "block_base.h"
#include "text_block.h"

namespace eerie_leap::subsys::mdf::mdf4 {

using namespace eerie_leap::subsys::mdf::utilities;

class ChannelConversionBlock : public BlockBase {
public:
    enum class ConversionType : uint8_t {
        /** \brief 1:1 conversion. No parameters needed. */
        NoConversion = 0,

        /** \brief Linear conversion. 2 parameters.
        * Eng = Ch * Par(1) + Par(0).
        */
        Linear = 1,

        /** \brief Rational function conversion. 6 parameters.
        *  Eng = (Par(0)*Ch^2 + Par(1)*Ch + Par(2)) /
        *  (Par(3)*Ch^2 + Par(4)*Ch + Par(5))
        */
        Rational = 2,
        Algebraic = 3, ///< Text formula.

        /** \brief Value to value conversion with interpolation.
        * Defined by a list of Key value pairs.
        * Par(n) = key and Par(n+1) value.
        */
        ValueToValueInterpolation = 4,

        /** \brief Value to value conversion without interpolation.
        * Defined by a list of Key value pairs.
        * Par(n) = key and Par(n+1) value.
        */
        ValueToValue = 5,

        /** \brief Value range to value conversion without interpolation.
        * Defined by a list of Key min/max value triplets.
        * Par(3*n) = key min, Par(3*(n+1)) = key max and Par(3*(n+2)) value. Add a default
        * value last, after all the triplets.
        */
        ValueRangeToValue = 6,

        /** \brief Value to text conversion.
        * Defined by a list of key values to text string conversions. This is
        * normally used for enumerated channels.
        * Par(n) value to Ref(n) text. Add a default
        * referenced text last.
        */
        ValueToText = 7,

        /** \brief Value range to text conversion.
        * Defined by a list of key min/max values to text string conversions. This is
        * normally used for enumerated channels.
        * Par(2*n) min key, Par(2(n+1)) max key to Ref(n) value. Add a default
        * referenced text  last.
        */
        ValueRangeToText = 8,

        /** \brief Text to value conversion.
        * Defines a list of text string to value conversion.
        * Ref(n) key to Par(n) value. Add a default value last to the parameter list.
        */
        TextToValue = 9,

        /** \brief Text to text conversion.
            * Defines a list of text string to text conversion.
            * Ref(2*n) key to Ref(2*(n+1)) value.
            * Add a text value last to the parameter list.
            */
        TextToTranslation = 10,

        /** \brief Bitfield to text conversion
        * Currently unsupported conversion.
        */
        BitfieldToText = 11
    };

    enum class Flag: uint16_t {
        Default = 0x00,
        PrecisionValid = 0x0001,    // Precision is used.
        RangeValid = 0x0002,        // Range is used.
        StatusString = 0x0004,      // Status string flag.
    };

private:
    enum class LinkType: int {
        TextName = 0,
        TextUnit,
        MetadataComment,
        ChannelConversionInverse    // Inverse channel conversion
    };

    BlockLinks<LinkType, 4> links_;
    // BlockLink references_[reference_count_]; // 8 bytes * reference_count_, reference parameters

    ConversionType conversion_type_;    // 1 bytes, conversion type
    uint8_t precision_;                 // 1 bytes, precision
    uint16_t flags_;                    // 2 bytes, flags
    uint16_t reference_count_;          // 2 bytes, number of reference parameters
    uint16_t value_count_;              // 2 bytes, number of value parameters
    uint64_t min_phisical_value_;       // 8 bytes, minimum physical value
    uint64_t max_phisical_value_;       // 8 bytes, maximum physical value
    std::vector<uint64_t> values_;      // 8 bytes * value_count_, value parameters

    explicit ChannelConversionBlock(ConversionType conversion_type);

public:
    virtual ~ChannelConversionBlock() = default;

    static ChannelConversionBlock CreateAlgebraicConversion(const std::string& formula);

    uint64_t GetBlockSize() const override;
    std::unique_ptr<uint8_t[]> Serialize() const override;
    const IBlockLinks* GetBlockLinks() const override { return &links_; }
    std::vector<std::shared_ptr<ISerializableBlock>> GetChildren() const override {
        return links_.GetLinks();
    }
};

} // namespace eerie_leap::subsys::mdf::mdf4
