#pragma once

#include "subsys/dbc/dbcppp/include/Attribute.h"

namespace dbcppp
{
    class AttributeImpl final
        : public IAttribute
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        AttributeImpl(std::allocator_arg_t, allocator_type alloc);
        AttributeImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , IAttributeDefinition::EObjectType object_type
            , IAttribute::value_t value);

        AttributeImpl& operator=(const AttributeImpl&) noexcept = default;
        AttributeImpl& operator=(AttributeImpl&&) noexcept = default;
        AttributeImpl(AttributeImpl&&) noexcept = default;
        ~AttributeImpl() = default;

        AttributeImpl(AttributeImpl&& other, allocator_type alloc)
            : _name(std::move(other._name), alloc)
            , _object_type(other._object_type)
            , _value(other._value)
            , _allocator(alloc) {}

        AttributeImpl(const AttributeImpl& other, allocator_type alloc = {}) noexcept
            : _name(other._name, alloc)
            , _object_type(other._object_type)
            , _value(other._value)
            , _allocator(alloc) {}

        virtual std::unique_ptr<IAttribute> Clone() const override;

        virtual const std::string_view Name() const override;
        virtual IAttributeDefinition::EObjectType ObjectType() const override;
        virtual const value_t& Value() const override;

        virtual bool operator==(const IAttribute& rhs) const override;
        virtual bool operator!=(const IAttribute& rhs) const override;

    private:
        std::pmr::string _name;
        IAttributeDefinition::EObjectType _object_type;
        IAttribute::value_t _value;

        allocator_type _allocator;
    };
}
