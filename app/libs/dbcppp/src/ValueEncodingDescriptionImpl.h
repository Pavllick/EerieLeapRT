
#pragma once

#include <string>
#include "dbcppp/ValueEncodingDescription.h"

namespace dbcppp
{
    class ValueEncodingDescriptionImpl
        : public IValueEncodingDescription
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        ValueEncodingDescriptionImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , int64_t value
            , std::pmr::string&& description);

        ValueEncodingDescriptionImpl& operator=(const ValueEncodingDescriptionImpl&) noexcept = default;
        ValueEncodingDescriptionImpl& operator=(ValueEncodingDescriptionImpl&&) noexcept = default;
        ValueEncodingDescriptionImpl(ValueEncodingDescriptionImpl&&) noexcept = default;
        ~ValueEncodingDescriptionImpl() = default;

        ValueEncodingDescriptionImpl(ValueEncodingDescriptionImpl&& other, allocator_type alloc)
            : _value(other._value)
            , _description(std::move(other._description), alloc)
            , _allocator(alloc) {}

        ValueEncodingDescriptionImpl(const ValueEncodingDescriptionImpl& other, allocator_type alloc = {})
            : _value(other._value)
            , _description(other._description, alloc)
            , _allocator(alloc) {}

        pmr_unique_ptr<IValueEncodingDescription> Clone() const override;

        int64_t Value() const override;
        const std::string_view Description() const override;

        bool operator==(const IValueEncodingDescription& rhs) const override;
        bool operator!=(const IValueEncodingDescription& rhs) const override;

    private:
        int64_t _value;
        std::pmr::string _description;

        allocator_type _allocator;
    };
}
