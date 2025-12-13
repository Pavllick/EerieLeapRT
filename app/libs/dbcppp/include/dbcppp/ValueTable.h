#pragma once

#include <memory>
#include <optional>
#include <functional>
#include <memory_resource>
#include <vector>
#include <string>
#include <optional>

#include "SignalType.h"
#include "ValueEncodingDescription.h"

namespace dbcppp
{
    class SignalType;
    class ValueTable final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        ValueTable(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , std::optional<SignalType>&& signal_type
            , std::pmr::vector<ValueEncodingDescription>&& value_encoding_descriptions);

        ValueTable& operator=(const ValueTable&) noexcept = default;
        ValueTable& operator=(ValueTable&&) noexcept = default;
        ValueTable(ValueTable&&) noexcept = default;
        ~ValueTable() = default;

        ValueTable(ValueTable&& other, allocator_type alloc)
            : _name(std::move(other._name), alloc)
            , _signal_type(std::move(other._signal_type))
            , _value_encoding_descriptions(std::move(other._value_encoding_descriptions), alloc)
            , _allocator(alloc) {}

        ValueTable(const ValueTable& other, allocator_type alloc = {})
            : _name(other._name, alloc)
            , _signal_type(other._signal_type)
            , _value_encoding_descriptions(other._value_encoding_descriptions, alloc)
            , _allocator(alloc) {}

        ValueTable Clone() const;

        const std::string_view Name() const;
        std::optional<std::reference_wrapper<const SignalType>> GetSignalType() const;
        const ValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const;
        std::size_t ValueEncodingDescriptions_Size() const;

        bool operator==(const ValueTable& rhs) const;
        bool operator!=(const ValueTable& rhs) const;

        DBCPPP_MAKE_ITERABLE(ValueTable, ValueEncodingDescriptions, ValueEncodingDescription);

    private:
        std::pmr::string _name;
        std::optional<SignalType> _signal_type;
        std::pmr::vector<ValueEncodingDescription> _value_encoding_descriptions;

        allocator_type _allocator;
    };
}
