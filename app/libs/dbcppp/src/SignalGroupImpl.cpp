#include <algorithm>
#include "SignalGroupImpl.h"

using namespace dbcppp;

pmr_unique_ptr<ISignalGroup> ISignalGroup::Create(
      std::pmr::memory_resource* mr
    , uint64_t message_id
    , std::pmr::string&& name
    , uint64_t repetitions
    , std::pmr::vector<std::pmr::string>&& signal_names)
{
    return make_unique_pmr<SignalGroupImpl>(
          mr
        , message_id
        , std::move(name)
        , repetitions
        , std::move(signal_names));
}

SignalGroupImpl::SignalGroupImpl(
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

pmr_unique_ptr<ISignalGroup> SignalGroupImpl::Clone() const
{
    return make_unique_pmr<SignalGroupImpl>(_allocator, *this);
}
uint64_t SignalGroupImpl::MessageId() const
{
    return _message_id;
}
const std::string_view SignalGroupImpl::Name() const
{
    return _name;
}
uint64_t SignalGroupImpl::Repetitions() const
{
    return _repetitions;
}
const std::pmr::string& SignalGroupImpl::SignalNames_Get(std::size_t i) const
{
    return _signal_names[i];
}
uint64_t SignalGroupImpl::SignalNames_Size() const
{
    return _signal_names.size();
}

bool SignalGroupImpl::operator==(const ISignalGroup& rhs) const
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
bool SignalGroupImpl::operator!=(const ISignalGroup& rhs) const
{
    return !(*this == rhs);
}
