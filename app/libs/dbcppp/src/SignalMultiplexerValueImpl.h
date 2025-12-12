
#pragma once

#include "dbcppp/SignalMultiplexerValue.h"

namespace dbcppp
{
    class SignalMultiplexerValueImpl
        : public ISignalMultiplexerValue
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        SignalMultiplexerValueImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& switch_name
            , std::pmr::vector<Range>&& value_ranges);

        SignalMultiplexerValueImpl(const SignalMultiplexerValueImpl&) = delete;
        SignalMultiplexerValueImpl& operator=(const SignalMultiplexerValueImpl&) = delete;
        SignalMultiplexerValueImpl& operator=(SignalMultiplexerValueImpl&&) noexcept = default;
        SignalMultiplexerValueImpl(SignalMultiplexerValueImpl&&) noexcept = default;
        ~SignalMultiplexerValueImpl() = default;

        SignalMultiplexerValueImpl(SignalMultiplexerValueImpl&& other, allocator_type alloc)
            : _switch_name(std::move(other._switch_name), alloc)
            , _value_ranges(std::move(other._value_ranges), alloc)
            , _allocator(alloc) {}

        SignalMultiplexerValueImpl(const SignalMultiplexerValueImpl& other, allocator_type alloc = {})
            : _switch_name(other._switch_name, alloc)
            , _value_ranges(other._value_ranges, alloc)
            , _allocator(alloc) {}

        virtual std::unique_ptr<ISignalMultiplexerValue> Clone() const override;

        virtual const std::string_view SwitchName() const override;
        virtual const Range& ValueRanges_Get(std::size_t i) const override;
        virtual uint64_t ValueRanges_Size() const override;

        virtual bool operator==(const ISignalMultiplexerValue& rhs) const override;
        virtual bool operator!=(const ISignalMultiplexerValue& rhs) const override;

    private:
        std::pmr::string _switch_name;
        std::pmr::vector<Range> _value_ranges;

        allocator_type _allocator;
    };
}
