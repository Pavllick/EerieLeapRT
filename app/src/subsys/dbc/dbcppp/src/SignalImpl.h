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
        using allocator_type = std::pmr::polymorphic_allocator<>;

        SignalImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , uint16_t message_size
            , std::pmr::string name
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
            , std::pmr::string unit
            , std::pmr::vector<std::pmr::string>&& receivers
            , std::pmr::vector<AttributeImpl>&& attribute_values
            , std::pmr::vector<ValueEncodingDescriptionImpl>&& value_encoding_descriptions
            , std::pmr::string comment
            , EExtendedValueType extended_value_type
            , std::pmr::vector<SignalMultiplexerValueImpl>&& signal_multiplexer_values);

        SignalImpl(const SignalImpl&) = delete;
        SignalImpl& operator=(const SignalImpl&) noexcept = default;
        SignalImpl& operator=(SignalImpl&&) noexcept = default;
        SignalImpl(SignalImpl &&) = default;
        ~SignalImpl() = default;

        SignalImpl(SignalImpl&& other, allocator_type alloc)
            : _name(std::move(other._name))
            , _multiplexer_indicator(other._multiplexer_indicator)
            , _multiplexer_switch_value(other._multiplexer_switch_value)
            , _start_bit(other._start_bit)
            , _bit_size(other._bit_size)
            , _byte_order(other._byte_order)
            , _value_type(other._value_type)
            , _factor(other._factor)
            , _offset(other._offset)
            , _minimum(other._minimum)
            , _maximum(other._maximum)
            , _unit(std::move(other._unit))
            , _comment(std::move(other._comment))
            , _extended_value_type(other._extended_value_type)
            , _signal_multiplexer_values(std::move(other._signal_multiplexer_values))
            , _mask(other._mask)
            , _mask_signed(other._mask_signed)
            , _fixed_start_bit_0(other._fixed_start_bit_0)
            , _fixed_start_bit_1(other._fixed_start_bit_1)
            , _byte_pos(other._byte_pos)
            , _error(other._error) {}

        const std::string_view Name() const override;
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
        const std::string_view Unit() const override;
        const std::pmr::string& Receivers_Get(std::size_t i) const override;
        uint64_t Receivers_Size() const override;
        const IValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const override;
        uint64_t ValueEncodingDescriptions_Size() const override;
        const IAttribute& AttributeValues_Get(std::size_t i) const override;
        uint64_t AttributeValues_Size() const override;
        const std::string_view Comment() const override;
        EExtendedValueType ExtendedValueType() const override;
        const ISignalMultiplexerValue& SignalMultiplexerValues_Get(std::size_t i) const override;
        uint64_t SignalMultiplexerValues_Size() const override;
        bool Error(EErrorCode code) const override;

        bool operator==(const ISignal& rhs) const override;
        bool operator!=(const ISignal& rhs) const override;

    private:
        void SetError(EErrorCode code);

        std::pmr::string _name;
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
        std::pmr::string _unit;
        std::pmr::vector<std::pmr::string> _receivers;
        std::pmr::vector<AttributeImpl> _attribute_values;
        std::pmr::vector<ValueEncodingDescriptionImpl> _value_encoding_descriptions;
        std::pmr::string _comment;
        EExtendedValueType _extended_value_type;
        std::pmr::vector<SignalMultiplexerValueImpl> _signal_multiplexer_values;

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
