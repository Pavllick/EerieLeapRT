#pragma once

#include <memory_resource>
#include <string>
#include <vector>
#include <variant>
#include <memory>

namespace dbcppp
{
    class AttributeDefinition final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        enum class EObjectType : std::uint8_t
        {
            Network,
            Node,
            Message,
            Signal,
            EnvironmentVariable,
        };
        struct ValueTypeInt
        {
            int64_t minimum;
            int64_t maximum;
        };
        struct ValueTypeHex
        {
            int64_t minimum;
            int64_t maximum;
        };
        struct ValueTypeFloat
        {
            double minimum;
            double maximum;
        };
        struct ValueTypeString
        {
        };
        struct ValueTypeEnum
        {
            // NOTE: Due to how this object is used, it is not possible to pass an allocator to the vector.
            std::pmr::vector<std::pmr::string> values;
        };
        using value_type_t = std::variant<ValueTypeInt, ValueTypeHex, ValueTypeFloat, ValueTypeString, ValueTypeEnum>;

        AttributeDefinition(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , EObjectType object_type
            , value_type_t value_type);

        AttributeDefinition& operator=(const AttributeDefinition&) noexcept = default;
        AttributeDefinition& operator=(AttributeDefinition&&) noexcept = default;
        AttributeDefinition(AttributeDefinition&&) noexcept = default;
        ~AttributeDefinition() = default;

        AttributeDefinition(AttributeDefinition&& other, allocator_type alloc)
            : _name(std::move(other._name))
            , _object_type(other._object_type)
            , _value_type(other._value_type)
            , _allocator(alloc) {}

        AttributeDefinition(const AttributeDefinition& other, allocator_type alloc = {})
            : _name(other._name, alloc)
            , _object_type(other._object_type)
            , _value_type(other._value_type)
            , _allocator(alloc) {}

        AttributeDefinition Clone() const;

        EObjectType ObjectType() const;
        const std::string_view Name() const;
        const value_type_t& ValueType() const;

        bool operator==(const AttributeDefinition& rhs) const;
        bool operator!=(const AttributeDefinition& rhs) const;

    private:
        std::pmr::string _name;
        EObjectType _object_type;
        value_type_t _value_type;

        allocator_type _allocator;
    };
}
