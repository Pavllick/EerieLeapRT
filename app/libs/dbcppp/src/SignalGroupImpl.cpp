#include <algorithm>
#include <memory>
#include "dbcppp/SignalGroup.h"

using namespace dbcppp;

SignalGroup::SignalGroup(
      std::allocator_arg_t
    , allocator_type alloc
    , uint64_t message_id
    , std::pmr::string&& name
    , uint64_t repetitions
    , std::pmr::vector<std::pmr::string>&& signal_names)

    : _message_id(message_id)
    , _name(std::move(name))
    , _repetitions(repetitions)
    , _signal_names(std::move(signal_names))
    , _allocator(alloc) {}

SignalGroup SignalGroup::Clone() const
{
    return {*this, _allocator};
}
uint64_t SignalGroup::MessageId() const
{
    return _message_id;
}
const std::string_view SignalGroup::Name() const
{
    return _name;
}
uint64_t SignalGroup::Repetitions() const
{
    return _repetitions;
}
const std::pmr::string& SignalGroup::SignalNames_Get(std::size_t i) const
{
    return _signal_names[i];
}
std::size_t SignalGroup::SignalNames_Size() const
{
    return _signal_names.size();
}

bool SignalGroup::operator==(const SignalGroup& rhs) const
{
    bool equal = true;
    equal &= _message_id == rhs.MessageId();
    equal &= _name == rhs.Name();
    equal &= _repetitions == rhs.Repetitions();
    for (const auto& signal_name : rhs.SignalNames())
    {
        auto beg = _signal_names.begin();
        auto end = _signal_names.end();
        equal &= std::ranges::find(beg, end, signal_name) != end;
    }
    return equal;
}
bool SignalGroup::operator!=(const SignalGroup& rhs) const
{
    return !(*this == rhs);
}
