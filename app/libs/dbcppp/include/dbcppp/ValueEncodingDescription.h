#pragma once

#include <memory_resource>
#include <string>

namespace dbcppp
{
    class ValueEncodingDescription
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        ValueEncodingDescription(
              std::allocator_arg_t
            , allocator_type alloc
            , int64_t value
            , std::pmr::string&& description);

        ValueEncodingDescription& operator=(const ValueEncodingDescription&) noexcept = default;
        ValueEncodingDescription& operator=(ValueEncodingDescription&&) noexcept = default;
        ValueEncodingDescription(ValueEncodingDescription&&) noexcept = default;
        ~ValueEncodingDescription() = default;

        ValueEncodingDescription(ValueEncodingDescription&& other, allocator_type alloc)
            : _value(other._value)
            , _description(std::move(other._description), alloc)
            , _allocator(alloc) {}

        ValueEncodingDescription(const ValueEncodingDescription& other, allocator_type alloc = {})
            : _value(other._value)
            , _description(other._description, alloc)
            , _allocator(alloc) {}

        ValueEncodingDescription Clone() const;

        int64_t Value() const;
        const std::string_view Description() const;

        bool operator==(const ValueEncodingDescription& rhs) const;
        bool operator!=(const ValueEncodingDescription& rhs) const;

    private:
        int64_t _value;
        std::pmr::string _description;

        allocator_type _allocator;
    };
}
