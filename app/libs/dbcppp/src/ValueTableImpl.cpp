#include <algorithm>
#include "dbcppp/ValueTable.h"

using namespace dbcppp;

ValueTable::ValueTable(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , std::optional<SignalType>&& signal_type
    , std::pmr::vector<ValueEncodingDescription>&& value_encoding_descriptions)

    : _name(std::move(name))
    , _signal_type(signal_type.has_value() ? std::move(signal_type) : std::nullopt)
    , _value_encoding_descriptions(std::move(value_encoding_descriptions))
    , _allocator(alloc)
{}
ValueTable ValueTable::Clone() const
{
    return {*this, _allocator};
}
const std::string_view ValueTable::Name() const
{
    return _name;
}
std::optional<std::reference_wrapper<const SignalType>> ValueTable::GetSignalType() const
{
    std::optional<std::reference_wrapper<const SignalType>> signal_type;
    if (_signal_type)
    {
        signal_type = *_signal_type;
    }
    return signal_type;
}
const ValueEncodingDescription& ValueTable::ValueEncodingDescriptions_Get(std::size_t i) const
{
    return _value_encoding_descriptions[i];
}
std::size_t ValueTable::ValueEncodingDescriptions_Size() const
{
    return _value_encoding_descriptions.size();
}
bool ValueTable::operator==(const ValueTable& rhs) const
{
    bool equal = true;
    equal &= _name == rhs.Name();
    equal &= _signal_type == rhs.GetSignalType();
    for (const auto& ved : rhs.ValueEncodingDescriptions())
    {
        auto beg = _value_encoding_descriptions.begin();
        auto end = _value_encoding_descriptions.end();
        equal &= std::ranges::find(beg, end, ved) != end;
    }
    return equal;
}
bool ValueTable::operator!=(const ValueTable& rhs) const
{
    return !(*this == rhs);
}
