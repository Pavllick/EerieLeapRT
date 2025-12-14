#pragma once

#include <memory_resource>
#include <cstddef>
#include <string>
#include <memory>

#include "Iterator.h"
#include "Signal.h"
#include "Attribute.h"
#include "SignalGroup.h"

namespace dbcppp
{
    class Message final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        enum class EErrorCode
            : uint64_t
        {
            NoError,
            MuxValeWithoutMuxSignal
        };

        Message(
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
            , std::pmr::vector<SignalGroup>&& signal_groups);

        Message& operator=(const Message&) noexcept = delete;
        Message& operator=(Message&&) noexcept = default;
        Message(Message&&) noexcept = default;
        ~Message() = default;

        Message(Message&& other, allocator_type alloc)
            : _id(other._id)
            , _name(std::move(other._name), alloc)
            , _message_size(other._message_size)
            , _transmitter(std::move(other._transmitter), alloc)
            , _message_transmitters(std::move(other._message_transmitters), alloc)
            , _signals(std::move(other._signals), alloc)
            , _attribute_values(std::move(other._attribute_values), alloc)
            , _comment(std::move(other._comment), alloc)
            , _signal_groups(std::move(other._signal_groups), alloc)
            , _mux_signal(other._mux_signal)
            , _error(other._error)
            , _allocator(alloc) {}

        Message(const Message& other, allocator_type alloc = {})
            : _id(other._id)
            , _name(other._name, alloc)
            , _message_size(other._message_size)
            , _transmitter(other._transmitter, alloc)
            , _message_transmitters(other._message_transmitters, alloc)
            , _signals(other._signals, alloc)
            , _attribute_values(other._attribute_values, alloc)
            , _comment(other._comment, alloc)
            , _signal_groups(other._signal_groups, alloc)
            , _mux_signal(nullptr)
            , _error(other._error)
            , _allocator(alloc)
        {
            for (const auto& sig : _signals)
            {
                switch (sig.MultiplexerIndicator())
                {
                case Signal::EMultiplexer::MuxSwitch:
                    _mux_signal = &sig;
                    break;
                }
            }
        }

        Message Clone() const;

        uint64_t Id() const;
        const std::string_view Name() const;
        uint64_t MessageSize() const;
        const std::string_view Transmitter() const;
        const std::pmr::string& MessageTransmitters_Get(std::size_t i) const;
        std::size_t MessageTransmitters_Size() const;
        const Signal& Signals_Get(std::size_t i) const;
        std::size_t Signals_Size() const;
        const Attribute& AttributeValues_Get(std::size_t i) const;
        std::size_t AttributeValues_Size() const;
        const std::string_view Comment() const;
        const SignalGroup& SignalGroups_Get(std::size_t i) const;
        std::size_t SignalGroups_Size() const;
        const Signal* MuxSignal() const;

        EErrorCode Error() const;

        const std::pmr::vector<Signal>& signals() const;

        bool operator==(const Message& rhs) const;
        bool operator!=(const Message& rhs) const;

        DBCPPP_MAKE_ITERABLE(Message, MessageTransmitters, std::pmr::string);
        DBCPPP_MAKE_ITERABLE(Message, Signals, Signal);
        DBCPPP_MAKE_ITERABLE(Message, AttributeValues, Attribute);
        DBCPPP_MAKE_ITERABLE(Message, SignalGroups, SignalGroup);

        void AddSignal(Signal&& sig);

        std::pmr::vector<Signal>& GetSignals() {
            return _signals;
        }

    private:
        uint64_t _id;
        std::pmr::string _name;
        uint64_t _message_size;
        std::pmr::string _transmitter;
        std::pmr::vector<std::pmr::string> _message_transmitters;
        std::pmr::vector<Signal> _signals;
        std::pmr::vector<Attribute> _attribute_values;
        std::pmr::string _comment;
        std::pmr::vector<SignalGroup> _signal_groups;

        const Signal* _mux_signal;

        EErrorCode _error;

        allocator_type _allocator;
    };
}
