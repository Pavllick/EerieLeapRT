#pragma once

#include <memory>
#include <optional>
#include <functional>

#include "dbcppp/ValueTable.h"
#include "SignalTypeImpl.h"
#include "ValueEncodingDescriptionImpl.h"

namespace dbcppp
{
    class SignalType;
    class ValueTableImpl final
        : public IValueTable
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        ValueTableImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , std::optional<SignalTypeImpl>&& signal_type
            , std::pmr::vector<ValueEncodingDescriptionImpl>&& value_encoding_descriptions);

        ValueTableImpl& operator=(const ValueTableImpl&) noexcept = default;
        ValueTableImpl& operator=(ValueTableImpl&&) noexcept = default;
        ValueTableImpl(ValueTableImpl&&) noexcept = default;
        ~ValueTableImpl() = default;

        ValueTableImpl(ValueTableImpl&& other, allocator_type alloc)
            : _name(std::move(other._name), alloc)
            , _signal_type(std::move(other._signal_type))
            , _value_encoding_descriptions(std::move(other._value_encoding_descriptions), alloc)
            , _allocator(alloc) {}

        ValueTableImpl(const ValueTableImpl& other, allocator_type alloc = {})
            : _name(other._name, alloc)
            , _signal_type(other._signal_type)
            , _value_encoding_descriptions(other._value_encoding_descriptions, alloc)
            , _allocator(alloc) {}

        pmr_unique_ptr<IValueTable> Clone() const override;

        const std::string_view Name() const override;
        std::optional<std::reference_wrapper<const ISignalType>> SignalType() const override;
        const IValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const override;
        uint64_t ValueEncodingDescriptions_Size() const override;

        bool operator==(const IValueTable& rhs) const override;
        bool operator!=(const IValueTable& rhs) const override;

    private:
        std::pmr::string _name;
        std::optional<SignalTypeImpl> _signal_type;
        std::pmr::vector<ValueEncodingDescriptionImpl> _value_encoding_descriptions;

        allocator_type _allocator;
    };
}
