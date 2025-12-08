#include <algorithm>
#include "MessageImpl.h"
#include "utilities/memory/heap_allocator.h"

using namespace dbcppp;


ext_unique_ptr<IMessage> IMessage::Create(
      uint64_t id
    , std::string&& name
    , uint64_t message_size
    , std::string&& transmitter
    , std::vector<std::string>&& message_transmitters
    , std::vector<ext_unique_ptr<ISignal>>&& signals_
    , std::vector<std::unique_ptr<IAttribute>>&& attribute_values
    , std::string&& comment
    , std::vector<std::unique_ptr<ISignalGroup>>&& signal_groups)
{
    std::vector<ext_unique_ptr<SignalImpl>> ss;
    std::vector<AttributeImpl> avs;
    std::vector<SignalGroupImpl> sgs;
    for (auto& s : signals_)
    {
        ss.push_back(make_unique_ext<SignalImpl>(std::move(static_cast<SignalImpl&>(*s))));
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
    return make_unique_ext<MessageImpl>(
          id
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
      uint64_t id
    , std::string&& name
    , uint64_t message_size
    , std::string&& transmitter
    , std::vector<std::string>&& message_transmitters
    , std::vector<ext_unique_ptr<SignalImpl>>&& signals_
    , std::vector<AttributeImpl>&& attribute_values
    , std::string&& comment
    , std::vector<SignalGroupImpl>&& signal_groups)

    : _id(id)
    , _name(std::move(name))
    , _message_size(message_size)
    , _transmitter(std::move(transmitter))
    , _message_transmitters(std::move(message_transmitters))
    , _signals(std::move(signals_))
    , _attribute_values(std::move(attribute_values))
    , _comment(std::move(comment))
    , _signal_groups(std::move(signal_groups))
    , _mux_signal(nullptr)
    , _error(EErrorCode::NoError)
{
    bool have_mux_value = false;
    for (const auto& sig : _signals)
    {
        switch (sig->MultiplexerIndicator())
        {
        case ISignal::EMultiplexer::MuxValue:
            have_mux_value = true;
            break;
        case ISignal::EMultiplexer::MuxSwitch:
            _mux_signal = sig.get();
            break;
        }
    }
    if (have_mux_value && _mux_signal == nullptr)
    {
        _error = EErrorCode::MuxValeWithoutMuxSignal;
    }
}
uint64_t MessageImpl::Id() const
{
    return _id;
}
const std::string& MessageImpl::Name() const
{
    return _name;
}
uint64_t MessageImpl::MessageSize() const
{
    return _message_size;
}
const std::string& MessageImpl::Transmitter() const
{
    return _transmitter;
}
const std::string& MessageImpl::MessageTransmitters_Get(std::size_t i) const
{
    return _message_transmitters[i];
}
uint64_t MessageImpl::MessageTransmitters_Size() const
{
    return _message_transmitters.size();
}
const ISignal& MessageImpl::Signals_Get(std::size_t i) const
{
    return *_signals[i];
}
uint64_t MessageImpl::Signals_Size() const
{
    return _signals.size();
}
const IAttribute& MessageImpl::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
uint64_t MessageImpl::AttributeValues_Size() const
{
    return _attribute_values.size();
}
const std::string& MessageImpl::Comment() const
{
    return _comment;
}
const ISignalGroup& MessageImpl::SignalGroups_Get(std::size_t i) const
{
    return _signal_groups[i];
}
uint64_t MessageImpl::SignalGroups_Size() const
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

const std::vector<ext_unique_ptr<SignalImpl>>& MessageImpl::signals() const
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
        auto iter = std::ranges::find_if(_signals.begin(), _signals.end(),
        [&](const auto& signal)
        {
            return *signal == sig;
        });

        equal &= iter != _signals.end();
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
