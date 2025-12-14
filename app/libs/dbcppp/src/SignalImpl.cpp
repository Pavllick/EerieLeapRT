#include <memory>
#include <memory_resource>
#include <algorithm>
#include <limits>

#include "Helper.h"
#include "dbcppp/Signal.h"

using namespace dbcppp;

enum class Alignment
{
    size_inbetween_first_64_bit,
    signal_exceeds_64_bit_size_but_signal_fits_into_64_bit,
    signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit
};

template <Alignment aAlignment, Signal::EByteOrder aByteOrder, Signal::EValueType aValueType, Signal::EExtendedValueType aExtendedValueType>
Signal::raw_t template_decode(const Signal* sig, const void* nbytes) noexcept
{
    const Signal* sigi = static_cast<const Signal*>(sig);
    uint64_t data;
    if constexpr (aAlignment == Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit)
    {
        data = *reinterpret_cast<const uint64_t*>(&reinterpret_cast<const uint8_t*>(nbytes)[sigi->_byte_pos]);
        uint64_t data1 = reinterpret_cast<const uint8_t*>(nbytes)[sigi->_byte_pos + 8];
        if constexpr (aByteOrder == Signal::EByteOrder::BigEndian)
        {
            //native_to_big_inplace(data);
            native_to_big_inplace(data);
            data &= sigi->_mask;
            data <<= sigi->_fixed_start_bit_0;
            data1 >>= sigi->_fixed_start_bit_1;
            data |= data1;
        }
        else
        {
            //native_to_little_inplace(data);
            native_to_little_inplace(data);
            data >>= sigi->_fixed_start_bit_0;
            data1 &= sigi->_mask;
            data1 <<= sigi->_fixed_start_bit_1;
            data |= data1;
        }
        if constexpr (aExtendedValueType == Signal::EExtendedValueType::Float ||
            aExtendedValueType == Signal::EExtendedValueType::Double)
        {
            return data;
        }
        if constexpr (aValueType == Signal::EValueType::Signed)
        {
            if (data & sigi->_mask_signed)
            {
                data |= sigi->_mask_signed;
            }
        }
        return data;
    }
    else
    {
        if constexpr (aAlignment == Alignment::size_inbetween_first_64_bit)
        {
            data = *reinterpret_cast<const uint64_t*>(nbytes);
        }
        else
        {
            data = *reinterpret_cast<const uint64_t*>(&reinterpret_cast<const uint8_t*>(nbytes)[sigi->_byte_pos]);
        }
        if constexpr (aByteOrder == Signal::EByteOrder::BigEndian)
        {
            //native_to_big_inplace(data);
            native_to_big_inplace(data);
        }
        else
        {
            //native_to_little_inplace(data);
            native_to_little_inplace(data);
        }
        if constexpr (aExtendedValueType == Signal::EExtendedValueType::Double)
        {
            return data;
        }
        data >>= sigi->_fixed_start_bit_0;
    }
    data &= sigi->_mask;
    if constexpr (aExtendedValueType == Signal::EExtendedValueType::Float)
    {
        return data;
    }
    if constexpr (aValueType == Signal::EValueType::Signed)
    {
        // bit extending
        // trust the compiler to optimize this
        if (data & sigi->_mask_signed)
        {
            data |= sigi->_mask_signed;
        }
    }
    return data;
}

constexpr uint64_t enum_mask(Alignment a, Signal::EByteOrder bo, Signal::EValueType vt, Signal::EExtendedValueType evt)
{
    uint64_t result = 0;
    switch (a)
    {
    case Alignment::size_inbetween_first_64_bit:                                    result |= 0b1; break;
    case Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit:         result |= 0b10; break;
    case Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit: result |= 0b100; break;
    }
    switch (bo)
    {
    case Signal::EByteOrder::LittleEndian:                                           result |= 0b1000; break;
    case Signal::EByteOrder::BigEndian:                                              result |= 0b10000; break;
    }
    switch (vt)
    {
    case Signal::EValueType::Signed:                                                 result |= 0b100000; break;
    case Signal::EValueType::Unsigned:                                               result |= 0b1000000; break;
    }
    switch (evt)
    {
    case Signal::EExtendedValueType::Integer:                                        result |= 0b10000000; break;
    case Signal::EExtendedValueType::Float:                                          result |= 0b100000000; break;
    case Signal::EExtendedValueType::Double:                                         result |= 0b1000000000; break;
    }
    return result;
}
using decode_func_t = Signal::raw_t (*)(const Signal*, const void*) noexcept;
decode_func_t make_decode(Alignment a, Signal::EByteOrder bo, Signal::EValueType vt, Signal::EExtendedValueType evt)
{
    constexpr auto si64b            = Alignment::size_inbetween_first_64_bit;
    constexpr auto se64bsbsfi64b    = Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit;
    constexpr auto se64bsasdnfi64b  = Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit;
    constexpr auto le              = Signal::EByteOrder::LittleEndian;
    constexpr auto be              = Signal::EByteOrder::BigEndian;
    constexpr auto sig             = Signal::EValueType::Signed;
    constexpr auto usig            = Signal::EValueType::Unsigned;
    constexpr auto i       = Signal::EExtendedValueType::Integer;
    constexpr auto f       = Signal::EExtendedValueType::Float;
    constexpr auto d       = Signal::EExtendedValueType::Double;
    switch (enum_mask(a, bo, vt, evt))
    {
    case enum_mask(si64b, le, sig, i):            return template_decode<si64b, le, sig, i>;
    case enum_mask(si64b, le, sig, f):            return template_decode<si64b, le, sig, f>;
    case enum_mask(si64b, le, sig, d):            return template_decode<si64b, le, sig, d>;
    case enum_mask(si64b, le, usig, i):           return template_decode<si64b, le, usig, i>;
    case enum_mask(si64b, le, usig, f):           return template_decode<si64b, le, usig, f>;
    case enum_mask(si64b, le, usig, d):           return template_decode<si64b, le, usig, d>;
    case enum_mask(si64b, be, sig, i):            return template_decode<si64b, be, sig, i>;
    case enum_mask(si64b, be, sig, f):            return template_decode<si64b, be, sig, f>;
    case enum_mask(si64b, be, sig, d):            return template_decode<si64b, be, sig, d>;
    case enum_mask(si64b, be, usig, i):           return template_decode<si64b, be, usig, i>;
    case enum_mask(si64b, be, usig, f):           return template_decode<si64b, be, usig, f>;
    case enum_mask(si64b, be, usig, d):           return template_decode<si64b, be, usig, d>;
    case enum_mask(se64bsbsfi64b, le, sig, i):    return template_decode<se64bsbsfi64b, le, sig, i>;
    case enum_mask(se64bsbsfi64b, le, sig, f):    return template_decode<se64bsbsfi64b, le, sig, f>;
    case enum_mask(se64bsbsfi64b, le, sig, d):    return template_decode<se64bsbsfi64b, le, sig, d>;
    case enum_mask(se64bsbsfi64b, le, usig, i):   return template_decode<se64bsbsfi64b, le, usig, i>;
    case enum_mask(se64bsbsfi64b, le, usig, f):   return template_decode<se64bsbsfi64b, le, usig, f>;
    case enum_mask(se64bsbsfi64b, le, usig, d):   return template_decode<se64bsbsfi64b, le, usig, d>;
    case enum_mask(se64bsbsfi64b, be, sig, i):    return template_decode<se64bsbsfi64b, be, sig, i>;
    case enum_mask(se64bsbsfi64b, be, sig, f):    return template_decode<se64bsbsfi64b, be, sig, f>;
    case enum_mask(se64bsbsfi64b, be, sig, d):    return template_decode<se64bsbsfi64b, be, sig, d>;
    case enum_mask(se64bsbsfi64b, be, usig, i):   return template_decode<se64bsbsfi64b, be, usig, i>;
    case enum_mask(se64bsbsfi64b, be, usig, f):   return template_decode<se64bsbsfi64b, be, usig, f>;
    case enum_mask(se64bsbsfi64b, be, usig, d):   return template_decode<se64bsbsfi64b, be, usig, d>;
    case enum_mask(se64bsasdnfi64b, le, sig, i):  return template_decode<se64bsasdnfi64b, le, sig, i>;
    case enum_mask(se64bsasdnfi64b, le, sig, f):  return template_decode<se64bsasdnfi64b, le, sig, f>;
    case enum_mask(se64bsasdnfi64b, le, sig, d):  return template_decode<se64bsasdnfi64b, le, sig, d>;
    case enum_mask(se64bsasdnfi64b, le, usig, i): return template_decode<se64bsasdnfi64b, le, usig, i>;
    case enum_mask(se64bsasdnfi64b, le, usig, f): return template_decode<se64bsasdnfi64b, le, usig, f>;
    case enum_mask(se64bsasdnfi64b, le, usig, d): return template_decode<se64bsasdnfi64b, le, usig, d>;
    case enum_mask(se64bsasdnfi64b, be, sig, i):  return template_decode<se64bsasdnfi64b, be, sig, i>;
    case enum_mask(se64bsasdnfi64b, be, sig, f):  return template_decode<se64bsasdnfi64b, be, sig, f>;
    case enum_mask(se64bsasdnfi64b, be, sig, d):  return template_decode<se64bsasdnfi64b, be, sig, d>;
    case enum_mask(se64bsasdnfi64b, be, usig, i): return template_decode<se64bsasdnfi64b, be, usig, i>;
    case enum_mask(se64bsasdnfi64b, be, usig, f): return template_decode<se64bsasdnfi64b, be, usig, f>;
    case enum_mask(se64bsasdnfi64b, be, usig, d): return template_decode<se64bsasdnfi64b, be, usig, d>;
    }
    return nullptr;
}
decode_func_t make_decodeMuxSignal(Alignment a, Signal::EByteOrder bo, Signal::EValueType vt, Signal::EExtendedValueType evt)
{
    constexpr auto si64b            = Alignment::size_inbetween_first_64_bit;
    constexpr auto se64bsbsfi64b    = Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit;
    constexpr auto se64bsasdnfi64b  = Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit;
    constexpr auto le              = Signal::EByteOrder::LittleEndian;
    constexpr auto be              = Signal::EByteOrder::BigEndian;
    constexpr auto sig             = Signal::EValueType::Signed;
    constexpr auto usig            = Signal::EValueType::Unsigned;
    constexpr auto i       = Signal::EExtendedValueType::Integer;
    constexpr auto f       = Signal::EExtendedValueType::Float;
    constexpr auto d       = Signal::EExtendedValueType::Double;
    switch (enum_mask(a, bo, vt, evt))
    {
    case enum_mask(si64b, le, sig, i):            return template_decode<si64b, le, sig, i>;
    case enum_mask(si64b, le, sig, f):            return template_decode<si64b, le, sig, f>;
    case enum_mask(si64b, le, sig, d):            return template_decode<si64b, le, sig, d>;
    case enum_mask(si64b, le, usig, i):           return template_decode<si64b, le, usig, i>;
    case enum_mask(si64b, be, sig, i):            return template_decode<si64b, be, sig, i>;
    case enum_mask(si64b, be, usig, i):           return template_decode<si64b, be, usig, i>;
    case enum_mask(se64bsbsfi64b, le, sig, i):    return template_decode<se64bsbsfi64b, le, sig, i>;
    case enum_mask(se64bsbsfi64b, le, usig, i):   return template_decode<se64bsbsfi64b, le, usig, i>;
    case enum_mask(se64bsbsfi64b, be, sig, i):    return template_decode<se64bsbsfi64b, be, sig, i>;
    case enum_mask(se64bsbsfi64b, be, usig, i):   return template_decode<se64bsbsfi64b, be, usig, i>;
    case enum_mask(se64bsasdnfi64b, le, sig, i):  return template_decode<se64bsasdnfi64b, le, sig, i>;
    case enum_mask(se64bsasdnfi64b, le, usig, i): return template_decode<se64bsasdnfi64b, le, usig, i>;
    case enum_mask(se64bsasdnfi64b, be, sig, i):  return template_decode<se64bsasdnfi64b, be, sig, i>;
    case enum_mask(se64bsasdnfi64b, be, usig, i): return template_decode<se64bsasdnfi64b, be, usig, i>;
    }
    return nullptr;
}
void encode(const Signal* sig, Signal::raw_t raw, void* buffer) noexcept
{
    const Signal* sigi = static_cast<const Signal*>(sig);
    char* b = reinterpret_cast<char*>(buffer);
    if (sigi->ByteOrder() == Signal::EByteOrder::BigEndian)
    {
        uint64_t src = sigi->StartBit();
        uint64_t dst = sigi->BitSize() - 1;
        for (uint64_t i = 0; i < sigi->BitSize(); i++)
        {
            if (raw & (1ull << dst))
            {
                b[src / 8] |= 1ull << (src % 8);
            }
            else
            {
                b[src / 8] &= ~(1ull << (src % 8));
            }
            if ((src % 8) == 0)
            {
                src += 15;
            }
            else
            {
                src--;
            }
            dst--;
        }
    }
    else
    {
        uint64_t src = sigi->StartBit();
        uint64_t dst = 0;
        for (uint64_t i = 0; i < sigi->BitSize(); i++)
        {
            if (raw & (1ull << dst))
            {
                b[src / 8] |= 1ull << (src % 8);
            }
            else
            {
                b[src / 8] &= ~(1ull << (src % 8));
            }
            src++;
            dst++;
        }
    }
}
template <class T>
double raw_to_phys(const Signal* signal, Signal::raw_t raw) noexcept
{
    double draw = double(*reinterpret_cast<T*>(&raw));
    return draw * signal->Factor() + signal->Offset();
}
template <class T>
Signal::raw_t phys_to_raw(const Signal* signal, double phys) noexcept
{
    T result = T((phys - signal->Offset()) / signal->Factor());
    return *reinterpret_cast<Signal::raw_t*>(&result);
}

Signal::Signal(
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
    , std::pmr::vector<SignalMultiplexerValue>&& signal_multiplexer_values)

    : _name(std::move(name), alloc)
    , _multiplexer_indicator(multiplexer_indicator)
    , _multiplexer_switch_value(multiplexer_switch_value)
    , _start_bit(start_bit)
    , _bit_size(bit_size)
    , _byte_order(byte_order)
    , _value_type(value_type)
    , _factor(factor)
    , _offset(offset)
    , _minimum(minimum)
    , _maximum(maximum)
    , _unit(unit, alloc)
    , _receivers(std::move(receivers), alloc)
    , _attribute_values(std::move(attribute_values), alloc)
    , _value_encoding_descriptions(std::move(value_encoding_descriptions), alloc)
    , _comment(std::move(comment), alloc)
    , _extended_value_type(extended_value_type)
    , _signal_multiplexer_values(std::move(signal_multiplexer_values), alloc)
    , _error(EErrorCode::NoError)
    , _allocator(alloc)
{
    message_size = message_size < 8 ? 8 : message_size;
    // check for out of frame size error
    switch (byte_order)
    {
    case EByteOrder::LittleEndian:
        if ((start_bit + bit_size) > message_size * 8)
        {
            SetError(EErrorCode::SignalExceedsMessageSize);
        }
        break;
    case EByteOrder::BigEndian:
        uint16_t fsize = bit_size + (7 - (start_bit % 8));
        int16_t fstart = int16_t(start_bit) - (start_bit % 8);
        if (fstart + ((fsize - 1) / 8) * 8 >= message_size * 8)
        {
            SetError(EErrorCode::SignalExceedsMessageSize);
        }
        break;
    }
    switch (extended_value_type)
    {
    case EExtendedValueType::Float:
        if (bit_size != 32)
        {
            SetError(EErrorCode::WrongBitSizeForExtendedDataType);
        }
        break;
    case EExtendedValueType::Double:
        if (bit_size != 64)
        {
            SetError(EErrorCode::WrongBitSizeForExtendedDataType);
        }
        break;
    }
    if (extended_value_type == EExtendedValueType::Float && !std::numeric_limits<float>::is_iec559)
    {
            SetError(EErrorCode::MaschinesFloatEncodingNotSupported);
    }
    if (extended_value_type == EExtendedValueType::Double && !std::numeric_limits<double>::is_iec559)
    {
            SetError(EErrorCode::MaschinesDoubleEncodingNotSupported);
    }

    // save some additional values to speed up decoding
    // CAN signals can be upto 64 bits in size but doing a shift of more than 63 bytes is undefined behavior
    _mask =  (1ull << (_bit_size - 1ull) << 1ull) - 1;
    _mask_signed = ~((1ull << (_bit_size - 1ull)) - 1);

    _byte_pos = _start_bit / 8;

    uint16_t nbytes;
    if (_byte_order == EByteOrder::LittleEndian)
    {
        nbytes = (_start_bit % 8 + _bit_size + 7) / 8;
    }
    else
    {
        nbytes = (_bit_size + (7 - _start_bit % 8) + 7) / 8;
    }
    Alignment alignment = Alignment::size_inbetween_first_64_bit;
    // check whether the data is in the first 8 bytes
    // so we can optimize out one memory access
    if (_byte_pos + nbytes <= 8)
    {
        alignment = Alignment::size_inbetween_first_64_bit;
        if (_byte_order == EByteOrder::LittleEndian)
        {
            _fixed_start_bit_0 = _start_bit;
        }
        else
        {
            _fixed_start_bit_0 = (8 * (7 - (_start_bit / 8))) + (_start_bit % 8) - (_bit_size - 1);
        }
    }
    // check whether we can align the data on 64 bit
    else if (_byte_pos  % 8 + nbytes <= 8)
    {
        alignment = Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit;
        // align the byte pos on 64 bit
        _byte_pos -= _byte_pos % 8;
        _fixed_start_bit_0 = _start_bit - _byte_pos * 8;
        if (_byte_order == EByteOrder::BigEndian)
        {
            _fixed_start_bit_0 = (8 * (7 - (_fixed_start_bit_0 / 8))) + (_fixed_start_bit_0 % 8) - (_bit_size - 1);
        }
    }
    // we aren't able to align the data on 64 bit, so check whether the data fits into on uint64_t
    else if (nbytes <= 8)
    {
        alignment = Alignment::signal_exceeds_64_bit_size_but_signal_fits_into_64_bit;
        _fixed_start_bit_0 = _start_bit - _byte_pos * 8;
        if (_byte_order == EByteOrder::BigEndian)
        {
            _fixed_start_bit_0 = (8 * (7 - (_fixed_start_bit_0 / 8))) + (_fixed_start_bit_0 % 8) - (_bit_size - 1);
        }
    }
    // we aren't able to align the data on 64 bit, and we aren't able to fit the data into one uint64_t
    // so we have to compose the resulting value
    else
    {
        alignment = Alignment::signal_exceeds_64_bit_size_and_signal_does_not_fit_into_64_bit;
        if (_byte_order == EByteOrder::BigEndian)
        {
            uint16_t nbits_last_byte = (7 - _start_bit % 8) + _bit_size - 64;
            _fixed_start_bit_0 = nbits_last_byte;
            _fixed_start_bit_1 = 8 - nbits_last_byte;
            _mask = (1ull << (_start_bit % 8 + 57)) - 1;
        }
        else
        {
            _fixed_start_bit_0 = _start_bit - _byte_pos * 8;
            _fixed_start_bit_1 = 64 - _start_bit % 8;
            uint16_t nbits_last_byte = _bit_size + _start_bit % 8 - 64;
            _mask = (1ull << nbits_last_byte) - 1ull;
        }
    }

    _decode = ::make_decode(alignment, _byte_order, _value_type, _extended_value_type);
    _encode = ::encode;
    switch (_extended_value_type)
    {
    case EExtendedValueType::Integer:
        switch (_value_type)
        {
        case EValueType::Signed:
            _raw_to_phys = ::raw_to_phys<int64_t>;
            _phys_to_raw = ::phys_to_raw<int64_t>;
            break;
        case EValueType::Unsigned:
            _raw_to_phys = ::raw_to_phys<uint64_t>;
            _phys_to_raw = ::phys_to_raw<uint64_t>;
            break;
        }
        break;
    case EExtendedValueType::Float:
        _raw_to_phys = ::raw_to_phys<float>;
        _phys_to_raw = ::phys_to_raw<float>;
        break;
    case EExtendedValueType::Double:
        _raw_to_phys = ::raw_to_phys<double>;
        _phys_to_raw = ::phys_to_raw<double>;
        break;
    }
}
Signal Signal::Clone() const
{
    return {*this, _allocator};
}
const std::string_view Signal::Name() const
{
    return _name;
}
Signal::EMultiplexer Signal::MultiplexerIndicator() const
{
    return _multiplexer_indicator;
}
uint64_t Signal::MultiplexerSwitchValue() const
{
    return _multiplexer_switch_value;
}
uint16_t Signal::StartBit() const
{
    return _start_bit;
}
uint16_t Signal::BitSize() const
{
    return _bit_size;
}
Signal::EByteOrder Signal::ByteOrder() const
{
    return _byte_order;
}
Signal::EValueType Signal::ValueType() const
{
    return _value_type;
}
float Signal::Factor() const
{
    return _factor;
}
float Signal::Offset() const
{
    return _offset;
}
float Signal::Minimum() const
{
    return _minimum;
}
float Signal::Maximum() const
{
    return _maximum;
}
const std::string_view Signal::Unit() const
{
    return _unit;
}
const std::pmr::string& Signal::Receivers_Get(std::size_t i) const
{
    return _receivers[i];
}
std::size_t Signal::Receivers_Size() const
{
    return _receivers.size();
}
const ValueEncodingDescription& Signal::ValueEncodingDescriptions_Get(std::size_t i) const
{
    return _value_encoding_descriptions[i];
}
std::size_t Signal::ValueEncodingDescriptions_Size() const
{
    return _value_encoding_descriptions.size();
}
const Attribute& Signal::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
std::size_t Signal::AttributeValues_Size() const
{
    return _attribute_values.size();
}
const std::string_view Signal::Comment() const
{
    return _comment;
}
Signal::EExtendedValueType Signal::ExtendedValueType() const
{
    return _extended_value_type;
}
const SignalMultiplexerValue& Signal::SignalMultiplexerValues_Get(std::size_t i) const
{
    return _signal_multiplexer_values[i];
}
std::size_t Signal::SignalMultiplexerValues_Size() const
{
    return _signal_multiplexer_values.size();
}
bool Signal::Error(EErrorCode code) const
{
    return code == _error || (uint8_t(_error) & uint8_t(code));
}
void Signal::SetError(EErrorCode code)
{
    _error = EErrorCode(uint8_t(_error) | uint8_t(code));
}
bool Signal::operator==(const Signal& rhs) const
{
    bool equal = true;
    equal &= _name == rhs.Name();
    equal &= _multiplexer_indicator == rhs.MultiplexerIndicator();
    equal &= _multiplexer_switch_value == rhs.MultiplexerSwitchValue();
    equal &= _start_bit == rhs.StartBit();
    equal &= _bit_size == rhs.BitSize();
    equal &= _byte_order == rhs.ByteOrder();
    equal &= _value_type == rhs.ValueType();
    equal &= _factor == rhs.Factor();
    equal &= _offset == rhs.Offset();
    equal &= _minimum == rhs.Minimum();
    equal &= _maximum == rhs.Maximum();
    equal &= _unit == rhs.Unit();
    for (const auto& r : rhs.Receivers())
    {
        auto beg = _receivers.begin();
        auto end = _receivers.end();
        equal &= std::find(beg, end, r) != end;
    }
    for (const auto& attr : rhs.AttributeValues())
    {
        auto beg = _attribute_values.begin();
        auto end = _attribute_values.end();
        equal &= std::find(beg, end, attr) != end;
    }
    for (const auto& ved : rhs.ValueEncodingDescriptions())
    {
        auto beg = _value_encoding_descriptions.begin();
        auto end = _value_encoding_descriptions.end();
        equal &= std::find(beg, end, ved) != end;
    }
    equal &= _comment == rhs.Comment();
    equal &= _extended_value_type == rhs.ExtendedValueType();
    for (const auto& smv : rhs.SignalMultiplexerValues())
    {
        auto beg = _signal_multiplexer_values.begin();
        auto end = _signal_multiplexer_values.end();
        equal &= std::ranges::find(beg, end, smv) != end;
    }
    return equal;
}
bool Signal::operator!=(const Signal& rhs) const
{
    return !(*this == rhs);
}
