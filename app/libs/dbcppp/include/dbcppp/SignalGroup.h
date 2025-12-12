
#pragma once

#include <memory_resource>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <cstdint>

#include "Iterator.h"

#include <eerie_memory.hpp>

using namespace eerie_memory;

namespace dbcppp
{
    class ISignalGroup
    {
    public:
        static pmr_unique_ptr<ISignalGroup> Create(
              std::pmr::memory_resource* mr
            , uint64_t message_id
            , std::pmr::string&& name
            , uint64_t repetitions
            , std::pmr::vector<std::pmr::string>&& signal_names);

        virtual pmr_unique_ptr<ISignalGroup> Clone() const = 0;

        virtual ~ISignalGroup() = default;
        virtual uint64_t MessageId() const = 0;
        virtual const std::string_view Name() const = 0;
        virtual uint64_t Repetitions() const = 0;
        virtual const std::pmr::string& SignalNames_Get(std::size_t i) const = 0;
        virtual uint64_t SignalNames_Size() const = 0;

        DBCPPP_MAKE_ITERABLE(ISignalGroup, SignalNames, std::pmr::string);

        virtual bool operator==(const ISignalGroup& rhs) const = 0;
        virtual bool operator!=(const ISignalGroup& rhs) const = 0;
    };
}
