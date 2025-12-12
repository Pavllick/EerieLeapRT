
#pragma once

#include <memory_resource>
#include <string>
#include <cstddef>
#include <memory>
#include <vector>
#include <functional>
#include <cstdint>

#include "Iterator.h"

namespace dbcppp
{
    class ISignalMultiplexerValue
    {
    public:
        struct Range
        {
            std::size_t from;
            std::size_t to;
        };

        static std::unique_ptr<ISignalMultiplexerValue> Create(
              std::pmr::memory_resource* mr
            , std::pmr::string&& switch_name
            , std::pmr::vector<Range>&& value_ranges);

        virtual std::unique_ptr<ISignalMultiplexerValue> Clone() const = 0;

        virtual ~ISignalMultiplexerValue() = default;
        virtual const std::string_view SwitchName() const = 0;
        virtual const Range& ValueRanges_Get(std::size_t i) const = 0;
        virtual uint64_t ValueRanges_Size() const = 0;

        DBCPPP_MAKE_ITERABLE(ISignalMultiplexerValue, ValueRanges, Range);

        virtual bool operator==(const ISignalMultiplexerValue& rhs) const = 0;
        virtual bool operator!=(const ISignalMultiplexerValue& rhs) const = 0;
    };
}
