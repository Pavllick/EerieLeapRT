#include <algorithm>
#include "dbcppp/MessageImpl.h"

using namespace dbcppp;


pmr_unique_ptr<IMessage> IMessage::Create(
      std::pmr::memory_resource* mr
    , uint64_t id
    , std::pmr::string&& name
    , uint64_t message_size
    , std::pmr::string&& transmitter
    , std::pmr::vector<std::pmr::string>&& message_transmitters
    , std::pmr::vector<pmr_unique_ptr<ISignal>>&& signals_
    , std::pmr::vector<pmr_unique_ptr<IAttribute>>&& attribute_values
    , std::pmr::string&& comment
    , std::pmr::vector<pmr_unique_ptr<ISignalGroup>>&& signal_groups)
{
    std::pmr::vector<SignalImpl> ss(mr);
    std::pmr::vector<AttributeImpl> avs(mr);
    std::pmr::vector<SignalGroupImpl> sgs(mr);
    for (auto& s : signals_)
    {
        ss.push_back(std::move(static_cast<SignalImpl&>(*s)));
        s.reset(nullptr);
    }
    for (auto& av : attribute_values)
    {
        avs.push_back(std::move(static_cast<AttributeImpl&>(*av)));
        av.reset(nullptr);
    }
    for (auto& sg : signal_groups)
    {
        sgs.push_back(std::move(static_cast<SignalGroupImpl&>(*sg)));
        sg.reset(nullptr);
    }
    return make_unique_pmr<MessageImpl>(
        mr
        , id
        , std::move(name)
        , message_size
        , std::move(transmitter)
        , std::move(message_transmitters)
        , std::move(ss)
        , std::move(avs)
        , std::move(comment)
        , std::move(sgs));
}
MessageImpl::MessageImpl(
      std::allocator_arg_t
    , allocator_type alloc
    , uint64_t id
    , std::pmr::string&& name
    , uint64_t message_size
    , std::pmr::string&& transmitter
    , std::pmr::vector<std::pmr::string>&& message_transmitters
    , std::pmr::vector<SignalImpl>&& signals_
    , std::pmr::vector<AttributeImpl>&& attribute_values
    , std::pmr::string&& comment
    , std::pmr::vector<SignalGroupImpl>&& signal_groups)

    : _id(id)
    , _name(std::move(name), alloc)
    , _message_size(message_size)
    , _transmitter(std::move(transmitter), alloc)
    , _message_transmitters(std::move(message_transmitters), alloc)
    , _signals(std::move(signals_), alloc)
    , _attribute_values(std::move(attribute_values), alloc)
    , _comment(std::move(comment), alloc)
    , _signal_groups(std::move(signal_groups), alloc)
    , _mux_signal(nullptr)
    , _error(EErrorCode::NoError)
    , _allocator(alloc)
{
    bool have_mux_value = false;
    for (const auto& sig : _signals)
    {
        switch (sig.MultiplexerIndicator())
        {
        case ISignal::EMultiplexer::MuxValue:
            have_mux_value = true;
            break;
        case ISignal::EMultiplexer::MuxSwitch:
            _mux_signal = &sig;
            break;
        }
    }
    if (have_mux_value && _mux_signal == nullptr)
    {
        _error = EErrorCode::MuxValeWithoutMuxSignal;
    }
}
pmr_unique_ptr<IMessage> MessageImpl::Clone() const
{
    return make_unique_pmr<MessageImpl>(_allocator, *this);
}
uint64_t MessageImpl::Id() const
{
    return _id;
}
const std::string_view MessageImpl::Name() const
{
    return _name;
}
uint64_t MessageImpl::MessageSize() const
{
    return _message_size;
}
const std::string_view MessageImpl::Transmitter() const
{
    return _transmitter;
}
const std::pmr::string& MessageImpl::MessageTransmitters_Get(std::size_t i) const
{
    return _message_transmitters[i];
}
std::size_t MessageImpl::MessageTransmitters_Size() const
{
    return _message_transmitters.size();
}
const ISignal& MessageImpl::Signals_Get(std::size_t i) const
{
    return _signals[i];
}
std::size_t MessageImpl::Signals_Size() const
{
    return _signals.size();
}
const IAttribute& MessageImpl::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
std::size_t MessageImpl::AttributeValues_Size() const
{
    return _attribute_values.size();
}
const std::string_view MessageImpl::Comment() const
{
    return _comment;
}
const ISignalGroup& MessageImpl::SignalGroups_Get(std::size_t i) const
{
    return _signal_groups[i];
}
std::size_t MessageImpl::SignalGroups_Size() const
{
    return _signal_groups.size();
}
const ISignal* MessageImpl::MuxSignal() const
{
    return _mux_signal;
}
MessageImpl::EErrorCode MessageImpl::Error() const
{
    return _error;
}

const std::pmr::vector<SignalImpl>& MessageImpl::signals() const
{
    return _signals;
}
bool MessageImpl::operator==(const IMessage& rhs) const
{
    bool equal = true;
    equal &= _id == rhs.Id();
    equal &= _name == rhs.Name();
    equal &= _transmitter == rhs.Transmitter();
    for (const auto& msg_trans : rhs.MessageTransmitters())
    {
        auto beg = _message_transmitters.begin();
        auto end = _message_transmitters.end();
        equal &= std::ranges::find(beg, end, msg_trans) != _message_transmitters.end();
    }
    for (const auto& sig : rhs.Signals())
    {
        auto beg = _signals.begin();
        auto end = _signals.end();
        equal &= std::ranges::find(beg, end, sig) != _signals.end();
    }
    for (const auto& attr : rhs.AttributeValues())
    {
        auto beg = _attribute_values.begin();
        auto end = _attribute_values.end();
        equal &= std::ranges::find(beg, end, attr) != _attribute_values.end();
    }
    equal &= _comment == rhs.Comment();
    for (const auto& sg : rhs.SignalGroups())
    {
        auto beg = _signal_groups.begin();
        auto end = _signal_groups.end();
        equal &= std::ranges::find(beg, end, sg) != end;
    }
    return equal;
}
bool MessageImpl::operator!=(const IMessage& rhs) const
{
    return !(*this == rhs);
}
