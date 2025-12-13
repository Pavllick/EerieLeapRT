#pragma once

#include <memory_resource>
#include <string>
#include <variant>

#include "AttributeDefinition.h"

namespace dbcppp
{
    class Attribute final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        using hex_value_t = int64_t;
        using value_t = std::variant<int64_t, double, std::pmr::string>;

        Attribute(std::allocator_arg_t, allocator_type alloc);
        Attribute(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , AttributeDefinition::EObjectType object_type
            , Attribute::value_t value);

        Attribute& operator=(const Attribute&) noexcept = default;
        Attribute& operator=(Attribute&&) noexcept = default;
        Attribute(Attribute&&) noexcept = default;
        ~Attribute() = default;

        Attribute(Attribute&& other, allocator_type alloc)
            : _name(std::move(other._name), alloc)
            , _object_type(other._object_type)
            , _value(other._value)
            , _allocator(alloc) {}

        Attribute(const Attribute& other, allocator_type alloc = {}) noexcept
            : _name(other._name, alloc)
            , _object_type(other._object_type)
            , _value(other._value)
            , _allocator(alloc) {}

        Attribute Clone() const;

        const std::string_view Name() const;
        AttributeDefinition::EObjectType ObjectType() const;
        const value_t& Value() const;

        bool operator==(const Attribute& rhs) const;
        bool operator!=(const Attribute& rhs) const;

    private:
        std::pmr::string _name;
        AttributeDefinition::EObjectType _object_type;
        Attribute::value_t _value;

        allocator_type _allocator;
    };
}
