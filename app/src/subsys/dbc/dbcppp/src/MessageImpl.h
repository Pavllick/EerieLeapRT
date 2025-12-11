#pragma once

#include "subsys/dbc/dbcppp/include/Message.h"
#include "SignalImpl.h"
#include "NodeImpl.h"
#include "AttributeImpl.h"
#include "SignalGroupImpl.h"

namespace dbcppp
{
    class MessageImpl final
        : public IMessage
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        MessageImpl(
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
            , std::pmr::vector<SignalGroupImpl>&& signal_groups);

        MessageImpl(const MessageImpl&) = delete;
        MessageImpl& operator=(const MessageImpl&) noexcept = default;
        MessageImpl& operator=(MessageImpl&&) noexcept = default;
        MessageImpl(MessageImpl&&) noexcept = default;
        ~MessageImpl() = default;

        MessageImpl(MessageImpl&& other, allocator_type alloc)
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
            , _error(other._error) {}

        uint64_t Id() const override;
        const std::string_view Name() const override;
        uint64_t MessageSize() const override;
        const std::string_view Transmitter() const override;
        const std::pmr::string& MessageTransmitters_Get(std::size_t i) const override;
        uint64_t MessageTransmitters_Size() const override;
        const ISignal& Signals_Get(std::size_t i) const override;
        uint64_t Signals_Size() const override;
        const IAttribute& AttributeValues_Get(std::size_t i) const override;
        uint64_t AttributeValues_Size() const override;
        const std::string_view Comment() const override;
        const ISignalGroup& SignalGroups_Get(std::size_t i) const override;
        uint64_t SignalGroups_Size() const override;
        const ISignal* MuxSignal() const override;

        EErrorCode Error() const override;

        const std::pmr::vector<SignalImpl>& signals() const;

        bool operator==(const IMessage& rhs) const override;
        bool operator!=(const IMessage& rhs) const override;

    private:
        uint64_t _id;
        std::pmr::string _name;
        uint64_t _message_size;
        std::pmr::string _transmitter;
        std::pmr::vector<std::pmr::string> _message_transmitters;
        std::pmr::vector<SignalImpl> _signals;
        std::pmr::vector<AttributeImpl> _attribute_values;
        std::pmr::string _comment;
        std::pmr::vector<SignalGroupImpl> _signal_groups;

        const ISignal* _mux_signal;

        EErrorCode _error;
    };
}
