#pragma once

#include <string>
#include <memory>

#include "subsys/dbc/dbcppp/include/Signal.h"
#include "subsys/dbc/dbcppp/include/Node.h"
#include "AttributeImpl.h"
#include "SignalMultiplexerValueImpl.h"
#include "ValueEncodingDescriptionImpl.h"

namespace dbcppp
{
    class SignalImpl final
        : public ISignal
    {
    public:
        SignalImpl(
              uint16_t message_size
            , std::string name
            , EMultiplexer multiplexer_indicator
            , uint64_t multiplexer_switch_value
            , uint16_t start_bit
            , uint16_t bit_size
            , EByteOrder byte_order
            , EValueType value_type
            , float factor
            , float offset
            , float minimum
            , float maximum
            , std::string unit
            , std::vector<std::string>&& receivers
            , std::vector<AttributeImpl>&& attribute_values
            , std::vector<ValueEncodingDescriptionImpl>&& value_encoding_descriptions
            , std::string comment
            , EExtendedValueType extended_value_type
            , std::vector<SignalMultiplexerValueImpl>&& signal_multiplexer_values);

        const std::string& Name() const override;
        EMultiplexer MultiplexerIndicator() const override;
        uint64_t MultiplexerSwitchValue() const override;
        uint16_t StartBit() const override;
        uint16_t BitSize() const override;
        EByteOrder ByteOrder() const override;
        EValueType ValueType() const override;
        float Factor() const override;
        float Offset() const override;
        float Minimum() const override;
        float Maximum() const override;
        const std::string& Unit() const override;
        const std::string& Receivers_Get(std::size_t i) const override;
        uint64_t Receivers_Size() const override;
        const IValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const override;
        uint64_t ValueEncodingDescriptions_Size() const override;
        const IAttribute& AttributeValues_Get(std::size_t i) const override;
        uint64_t AttributeValues_Size() const override;
        const std::string& Comment() const override;
        EExtendedValueType ExtendedValueType() const override;
        const ISignalMultiplexerValue& SignalMultiplexerValues_Get(std::size_t i) const override;
        uint64_t SignalMultiplexerValues_Size() const override;
        bool Error(EErrorCode code) const override;

        bool operator==(const ISignal& rhs) const override;
        bool operator!=(const ISignal& rhs) const override;

    private:
        void SetError(EErrorCode code);

        std::string _name;
        EMultiplexer _multiplexer_indicator;
        uint64_t _multiplexer_switch_value;
        uint16_t _start_bit;
        uint16_t _bit_size;
        EByteOrder _byte_order;
        EValueType _value_type;
        float _factor;
        float _offset;
        float _minimum;
        float _maximum;
        std::string _unit;
        // std::vector<std::string> _receivers;
        // std::vector<AttributeImpl> _attribute_values;
        // std::vector<ValueEncodingDescriptionImpl> _value_encoding_descriptions;
        std::string _comment;
        EExtendedValueType _extended_value_type;
        std::vector<SignalMultiplexerValueImpl> _signal_multiplexer_values;

    public:
        // for performance
        uint64_t _mask;
        uint64_t _mask_signed;
        uint16_t _fixed_start_bit_0;
        uint16_t _fixed_start_bit_1;
        uint16_t _byte_pos;

        EErrorCode _error;
    };
}
