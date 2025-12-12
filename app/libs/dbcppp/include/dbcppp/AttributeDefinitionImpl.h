#pragma once

#include "dbcppp/AttributeDefinition.h"

namespace dbcppp
{
    class AttributeDefinitionImpl final
        : public IAttributeDefinition
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        AttributeDefinitionImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , EObjectType object_type
            , value_type_t value_type);

        AttributeDefinitionImpl& operator=(const AttributeDefinitionImpl&) noexcept = default;
        AttributeDefinitionImpl& operator=(AttributeDefinitionImpl&&) noexcept = default;
        AttributeDefinitionImpl(AttributeDefinitionImpl&&) noexcept = default;
        ~AttributeDefinitionImpl() = default;

        AttributeDefinitionImpl(AttributeDefinitionImpl&& other, allocator_type alloc)
            : _name(std::move(other._name))
            , _object_type(other._object_type)
            , _value_type(other._value_type)
            , _allocator(alloc) {}

        AttributeDefinitionImpl(const AttributeDefinitionImpl& other, allocator_type alloc = {})
            : _name(other._name, alloc)
            , _object_type(other._object_type)
            , _value_type(other._value_type)
            , _allocator(alloc) {}

        pmr_unique_ptr<IAttributeDefinition> Clone() const override;

        EObjectType ObjectType() const override;
        const std::string_view Name() const override;
        const value_type_t& ValueType() const override;

        bool operator==(const IAttributeDefinition& rhs) const override;
        bool operator!=(const IAttributeDefinition& rhs) const override;

    private:
        std::pmr::string _name;
        EObjectType _object_type;
        value_type_t _value_type;

        allocator_type _allocator;
    };
}
