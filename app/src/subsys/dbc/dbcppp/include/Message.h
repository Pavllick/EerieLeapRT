#pragma once

#include <cstddef>
#include <string>
#include <memory>

#include "subsys/dbc/dbcppp/include/Iterator.h"
#include "subsys/dbc/dbcppp/include/Signal.h"
#include "subsys/dbc/dbcppp/include/Attribute.h"
#include "subsys/dbc/dbcppp/include/SignalGroup.h"

#include "utilities/memory/heap_allocator.h"

using namespace eerie_leap::utilities::memory;

namespace dbcppp
{
    class IMessage
    {
    public:
        enum class EErrorCode
            : uint64_t
        {
            NoError,
            MuxValeWithoutMuxSignal
        };

        static ext_unique_ptr<IMessage> Create(
              uint64_t id
            , std::string&& name
            , uint64_t message_size
            , std::string&& transmitter
            , std::vector<std::string>&& message_transmitters
            , std::vector<ext_unique_ptr<ISignal>>&& signals_
            , std::vector<std::unique_ptr<IAttribute>>&& attribute_values
            , std::string&& comment
            , std::vector<std::unique_ptr<ISignalGroup>>&& signal_groups);

        virtual ~IMessage() = default;
        virtual uint64_t Id() const = 0;
        virtual const std::string& Name() const = 0;
        virtual uint64_t MessageSize() const = 0;
        virtual const std::string& Transmitter() const = 0;
        virtual const std::string& MessageTransmitters_Get(std::size_t i) const = 0;
        virtual uint64_t MessageTransmitters_Size() const = 0;
        virtual const ISignal& Signals_Get(std::size_t i) const = 0;
        virtual uint64_t Signals_Size() const = 0;
        virtual const IAttribute& AttributeValues_Get(std::size_t i) const = 0;
        virtual uint64_t AttributeValues_Size() const = 0;
        virtual const std::string& Comment() const = 0;
        virtual const ISignalGroup& SignalGroups_Get(std::size_t i) const = 0;
        virtual uint64_t SignalGroups_Size() const = 0;
        virtual const ISignal* MuxSignal() const = 0;

        DBCPPP_MAKE_ITERABLE(IMessage, MessageTransmitters, std::string);
        DBCPPP_MAKE_ITERABLE(IMessage, Signals, ISignal);
        DBCPPP_MAKE_ITERABLE(IMessage, AttributeValues, IAttribute);
        DBCPPP_MAKE_ITERABLE(IMessage, SignalGroups, ISignalGroup);

        virtual bool operator==(const IMessage& message) const = 0;
        virtual bool operator!=(const IMessage& message) const = 0;

        virtual EErrorCode Error() const = 0;
    };
}
