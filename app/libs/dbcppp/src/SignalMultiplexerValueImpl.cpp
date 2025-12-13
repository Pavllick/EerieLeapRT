#include <algorithm>
#include "dbcppp/SignalMultiplexerValue.h"

using namespace dbcppp;

SignalMultiplexerValue::SignalMultiplexerValue(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& switch_name
    , std::pmr::vector<Range>&& value_ranges)

    : _switch_name(std::move(switch_name), alloc)
    , _value_ranges(std::move(value_ranges), alloc)
    , _allocator(alloc)
{}

SignalMultiplexerValue SignalMultiplexerValue::Clone() const
{
    return {*this, _allocator};
}

const std::string_view SignalMultiplexerValue::SwitchName() const
{
    return _switch_name;
}
const SignalMultiplexerValue::Range& SignalMultiplexerValue::ValueRanges_Get(std::size_t i) const
{
    return _value_ranges[i];
}
std::size_t SignalMultiplexerValue::ValueRanges_Size() const
{
    return _value_ranges.size();
}

bool SignalMultiplexerValue::operator==(const SignalMultiplexerValue& rhs) const
{
    bool equal = true;
    equal &= _switch_name == rhs.SwitchName();
    for (const auto& range : rhs.ValueRanges())
    {
        auto beg = _value_ranges.begin();
        auto end = _value_ranges.end();
        equal &= std::find_if(beg, end,
            [&](const auto& other) { return range.from == other.from && range.to == other.to; }) != end;
    }
    return equal;
}
bool SignalMultiplexerValue::operator!=(const SignalMultiplexerValue& rhs) const
{
    return !(*this == rhs);
}
