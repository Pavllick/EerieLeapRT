#include <algorithm>
#include <memory>
#include "dbcppp/Message.h"

using namespace dbcppp;

Message::Message(
      std::allocator_arg_t
    , allocator_type alloc
    , uint64_t id
    , std::pmr::string&& name
    , uint64_t message_size
    , std::pmr::string&& transmitter
    , std::pmr::vector<std::pmr::string>&& message_transmitters
    , std::pmr::vector<Signal>&& signals_
    , std::pmr::vector<Attribute>&& attribute_values
    , std::pmr::string&& comment
    , std::pmr::vector<SignalGroup>&& signal_groups)

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
        case Signal::EMultiplexer::MuxValue:
            have_mux_value = true;
            break;
        case Signal::EMultiplexer::MuxSwitch:
            _mux_signal = &sig;
            break;
        }
    }
    if (have_mux_value && _mux_signal == nullptr)
    {
        _error = EErrorCode::MuxValeWithoutMuxSignal;
    }
}
Message Message::Clone() const
{
    return {*this, _allocator};
}
uint64_t Message::Id() const
{
    return _id;
}
const std::string_view Message::Name() const
{
    return _name;
}
uint64_t Message::MessageSize() const
{
    return _message_size;
}
const std::string_view Message::Transmitter() const
{
    return _transmitter;
}
const std::pmr::string& Message::MessageTransmitters_Get(std::size_t i) const
{
    return _message_transmitters[i];
}
std::size_t Message::MessageTransmitters_Size() const
{
    return _message_transmitters.size();
}
const Signal& Message::Signals_Get(std::size_t i) const
{
    return _signals[i];
}
std::size_t Message::Signals_Size() const
{
    return _signals.size();
}
const Attribute& Message::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
std::size_t Message::AttributeValues_Size() const
{
    return _attribute_values.size();
}
const std::string_view Message::Comment() const
{
    return _comment;
}
const SignalGroup& Message::SignalGroups_Get(std::size_t i) const
{
    return _signal_groups[i];
}
std::size_t Message::SignalGroups_Size() const
{
    return _signal_groups.size();
}
const Signal* Message::MuxSignal() const
{
    return _mux_signal;
}
Message::EErrorCode Message::Error() const
{
    return _error;
}

const std::pmr::vector<Signal>& Message::signals() const
{
    return _signals;
}
// Message& Message::operator=(const Message& other)
// {
//     _id = other._id;
//     _name = other._name;
//     _message_size = other._message_size;
//     _transmitter = other._transmitter;
//     _message_transmitters = other._message_transmitters;
//     _signals = other._signals;
//     _attribute_values = other._attribute_values;
//     _comment = other._comment;
//     _mux_signal = nullptr;
//     for (const auto& sig : _signals)
//     {
//         switch (sig.MultiplexerIndicator())
//         {
//         case Signal::EMultiplexer::MuxSwitch:
//             _mux_signal = &sig;
//             break;
//         }
//     }
//     _error = other._error;
//     return *this;
// }
bool Message::operator==(const Message& rhs) const
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
bool Message::operator!=(const Message& rhs) const
{
    return !(*this == rhs);
}
void Message::AddSignal(Signal&& signal) {
    _signals.push_back(std::move(signal));

    bool have_mux_value = false;
    for (const auto& sig : _signals)
    {
        switch (sig.MultiplexerIndicator())
        {
        case Signal::EMultiplexer::MuxValue:
            have_mux_value = true;
            break;
        case Signal::EMultiplexer::MuxSwitch:
            _mux_signal = &sig;
            break;
        }
    }
    if (have_mux_value && _mux_signal == nullptr)
    {
        _error = EErrorCode::MuxValeWithoutMuxSignal;
    }
}
