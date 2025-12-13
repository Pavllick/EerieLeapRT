
#pragma once

#include <memory_resource>
#include <string>
#include <cstddef>
#include <vector>

#include "Iterator.h"

namespace dbcppp
{
    class SignalMultiplexerValue
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        struct Range
        {
            uint64_t from;
            uint64_t to;
        };

        SignalMultiplexerValue(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& switch_name
            , std::pmr::vector<Range>&& value_ranges);

        SignalMultiplexerValue(const SignalMultiplexerValue&) = delete;
        SignalMultiplexerValue& operator=(const SignalMultiplexerValue&) = delete;
        SignalMultiplexerValue& operator=(SignalMultiplexerValue&&) noexcept = default;
        SignalMultiplexerValue(SignalMultiplexerValue&&) noexcept = default;
        ~SignalMultiplexerValue() = default;

        SignalMultiplexerValue(SignalMultiplexerValue&& other, allocator_type alloc)
            : _switch_name(std::move(other._switch_name), alloc)
            , _value_ranges(std::move(other._value_ranges), alloc)
            , _allocator(alloc) {}

        SignalMultiplexerValue(const SignalMultiplexerValue& other, allocator_type alloc = {})
            : _switch_name(other._switch_name, alloc)
            , _value_ranges(other._value_ranges, alloc)
            , _allocator(alloc) {}

        SignalMultiplexerValue Clone() const;

        const std::string_view SwitchName() const;
        const Range& ValueRanges_Get(std::size_t i) const;
        std::size_t ValueRanges_Size() const;

        bool operator==(const SignalMultiplexerValue& rhs) const;
        bool operator!=(const SignalMultiplexerValue& rhs) const;

        DBCPPP_MAKE_ITERABLE(SignalMultiplexerValue, ValueRanges, Range);

    private:
        std::pmr::string _switch_name;
        std::pmr::vector<Range> _value_ranges;

        allocator_type _allocator;
    };
}
