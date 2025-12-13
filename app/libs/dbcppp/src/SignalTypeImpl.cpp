#include "dbcppp/SignalType.h"
#include <memory>

using namespace dbcppp;

SignalType::SignalType(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , uint64_t signal_size
    , Signal::EByteOrder byte_order
    , Signal::EValueType value_type
    , double factor
    , double offset
    , double minimum
    , double maximum
    , std::pmr::string&& unit
    , double default_value
    , std::pmr::string&& value_table)

    : _name(std::move(name))
    , _signal_size(std::move(signal_size))
    , _byte_order(std::move(byte_order))
    , _value_type(std::move(value_type))
    , _factor(std::move(factor))
    , _offset(std::move(offset))
    , _minimum(std::move(minimum))
    , _maximum(std::move(maximum))
    , _unit(std::move(unit))
    , _default_value(std::move(default_value))
    , _value_table(std::move(value_table))
    , _allocator(alloc)
{}
SignalType SignalType::Clone() const
{
    return {*this, _allocator};
}
const std::string_view SignalType::Name() const
{
    return _name;
}
uint64_t SignalType::SignalSize() const
{
    return _signal_size;
}
Signal::EByteOrder SignalType::ByteOrder() const
{
    return _byte_order;
}
Signal::EValueType SignalType::ValueType() const
{
    return _value_type;
}
double SignalType::Factor() const
{
    return _factor;
}
double SignalType::Offset() const
{
    return _offset;
}
double SignalType::Minimum() const
{
    return _minimum;
}
double SignalType::Maximum() const
{
    return _maximum;
}
const std::string_view SignalType::Unit() const
{
    return _unit;
}
double SignalType::DefaultValue() const
{
    return _default_value;
}
const std::string_view SignalType::ValueTable() const
{
    return _value_table;
}
bool SignalType::operator==(const SignalType& rhs) const
{
    bool equal = true;
    equal &= _name == rhs.Name();
    equal &= _signal_size == rhs.SignalSize();
    equal &= _value_type == rhs.ValueType();
    equal &= _factor == rhs.Factor();
    equal &= _offset == rhs.Offset();
    equal &= _minimum == rhs.Minimum();
    equal &= _maximum == rhs.Maximum();
    equal &= _unit == rhs.Unit();
    equal &= _default_value == rhs.DefaultValue();
    equal &= _value_table == rhs.ValueTable();
    return equal;
}
bool SignalType::operator!=(const SignalType& rhs) const
{
    return !(*this == rhs);
}
