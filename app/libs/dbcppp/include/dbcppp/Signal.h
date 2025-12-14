#pragma once

#include <string>
#include <memory>
#include <string>
#include <cstddef>

#include "Iterator.h"
#include "Attribute.h"
#include "SignalMultiplexerValue.h"
#include "ValueEncodingDescription.h"

namespace dbcppp
{
    class Signal final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        enum class EErrorCode : uint8_t
        {
            NoError,
            MaschinesFloatEncodingNotSupported = 1,
            MaschinesDoubleEncodingNotSupported = 2,
            SignalExceedsMessageSize = 4,
            WrongBitSizeForExtendedDataType = 8
        };
        enum class EMultiplexer : uint8_t
        {
            NoMux, MuxSwitch, MuxValue
        };
        enum class EByteOrder : uint8_t
        {
            BigEndian = 0,
            LittleEndian = 1
        };
        enum class EValueType : uint8_t
        {
            Signed, Unsigned
        };
        enum class EExtendedValueType : uint8_t
        {
            Integer, Float, Double
        };

        Signal(
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
            , std::pmr::vector<Attribute>&& attribute_values
            , std::pmr::vector<ValueEncodingDescription>&& value_encoding_descriptions
            , std::pmr::string comment
            , EExtendedValueType extended_value_type
            , std::pmr::vector<SignalMultiplexerValue>&& signal_multiplexer_values);

        Signal& operator=(const Signal&) noexcept = default;
        Signal& operator=(Signal&&) noexcept = default;
        Signal(Signal &&) = default;
        ~Signal() = default;

        Signal(Signal&& other, allocator_type alloc)
            : _decode(other._decode)
            , _encode(other._encode)
            , _raw_to_phys(other._raw_to_phys)
            , _phys_to_raw(other._phys_to_raw)
            , _name(std::move(other._name))
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
            , _receivers(std::move(other._receivers), alloc)
            , _attribute_values(std::move(other._attribute_values), alloc)
            , _value_encoding_descriptions(std::move(other._value_encoding_descriptions), alloc)
            , _comment(std::move(other._comment))
            , _extended_value_type(other._extended_value_type)
            , _signal_multiplexer_values(std::move(other._signal_multiplexer_values))
            , _allocator(alloc)
            , _mask(other._mask)
            , _mask_signed(other._mask_signed)
            , _fixed_start_bit_0(other._fixed_start_bit_0)
            , _fixed_start_bit_1(other._fixed_start_bit_1)
            , _byte_pos(other._byte_pos)
            , _error(other._error) {}

        Signal(const Signal& other, allocator_type alloc = {})
            : _decode(other._decode)
            , _encode(other._encode)
            , _raw_to_phys(other._raw_to_phys)
            , _phys_to_raw(other._phys_to_raw)
            , _name(other._name)
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
            , _unit(other._unit)
            , _receivers(other._receivers, alloc)
            , _attribute_values(other._attribute_values, alloc)
            , _value_encoding_descriptions(other._value_encoding_descriptions, alloc)
            , _comment(other._comment)
            , _extended_value_type(other._extended_value_type)
            , _signal_multiplexer_values(other._signal_multiplexer_values)
            , _allocator(alloc)
            , _mask(other._mask)
            , _mask_signed(other._mask_signed)
            , _fixed_start_bit_0(other._fixed_start_bit_0)
            , _fixed_start_bit_1(other._fixed_start_bit_1)
            , _byte_pos(other._byte_pos)
            , _error(other._error) {}

        Signal Clone() const;

        const std::string_view Name() const;
        EMultiplexer MultiplexerIndicator() const;
        uint64_t MultiplexerSwitchValue() const;
        uint16_t StartBit() const;
        uint16_t BitSize() const;
        EByteOrder ByteOrder() const;
        EValueType ValueType() const;
        float Factor() const;
        float Offset() const;
        float Minimum() const;
        float Maximum() const;
        const std::string_view Unit() const;
        const std::pmr::string& Receivers_Get(std::size_t i) const;
        std::size_t Receivers_Size() const;
        const ValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const;
        std::size_t ValueEncodingDescriptions_Size() const;
        const Attribute& AttributeValues_Get(std::size_t i) const;
        std::size_t AttributeValues_Size() const;
        const std::string_view Comment() const;
        EExtendedValueType ExtendedValueType() const;
        const SignalMultiplexerValue& SignalMultiplexerValues_Get(std::size_t i) const;
        std::size_t SignalMultiplexerValues_Size() const;
        bool Error(EErrorCode code) const;

        bool operator==(const Signal& rhs) const;
        bool operator!=(const Signal& rhs) const;

        /// \brief Extracts the raw value from a given n byte array
        ///
        /// This function uses a optimized method of reversing the byte order and extracting
        /// the value from the given data.
        /// !!! Note: This function takes at least 8 bytes and at least as many as the signal needs to be represented,
        ///     if you pass less, the program ends up in undefined behaviour! !!!
        ///
        /// @param nbyte a n byte array (!!! at least 8 bytes !!!) which is representing the can data.
        ///               the data must be in this order:
        ///               bit_0  - bit_7:  bytes[0]
        ///               bit_8  - bit_15: bytes[1]
        ///               ...
        ///               bit_n-7 - bit_n: bytes[n / 8]
        ///               (like the Unix CAN frame does store the data)
        using raw_t = uint64_t;
        inline raw_t Decode(const void* bytes) const noexcept { return _decode(this, bytes); }
        inline void Encode(raw_t raw, void* buffer) const noexcept { return _encode(this, raw, buffer); }

        inline double RawToPhys(raw_t raw) const noexcept { return _raw_to_phys(this, raw); }
        inline raw_t PhysToRaw(double phys) const noexcept { return _phys_to_raw(this, phys); }

        DBCPPP_MAKE_ITERABLE(Signal, Receivers, std::pmr::string);
        DBCPPP_MAKE_ITERABLE(Signal, ValueEncodingDescriptions, ValueEncodingDescription);
        DBCPPP_MAKE_ITERABLE(Signal, AttributeValues, Attribute);
        DBCPPP_MAKE_ITERABLE(Signal, SignalMultiplexerValues, SignalMultiplexerValue);

    protected:
        // instead of using virtuals dynamic dispatching use function pointers
        raw_t (*_decode)(const Signal* sig, const void* bytes) noexcept {nullptr};
        void (*_encode)(const Signal* sig, raw_t raw, void* buffer) noexcept {nullptr};
        double (*_raw_to_phys)(const Signal* sig, raw_t raw) noexcept {nullptr};
        raw_t (*_phys_to_raw)(const Signal* sig, double phys) noexcept {nullptr};

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
        std::pmr::vector<Attribute> _attribute_values;
        std::pmr::vector<ValueEncodingDescription> _value_encoding_descriptions;
        std::pmr::string _comment;
        EExtendedValueType _extended_value_type;
        std::pmr::vector<SignalMultiplexerValue> _signal_multiplexer_values;

        allocator_type _allocator;

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
