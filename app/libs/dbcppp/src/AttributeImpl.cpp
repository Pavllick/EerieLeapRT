#include "dbcppp/Attribute.h"

using namespace dbcppp;

Attribute::Attribute(std::allocator_arg_t, allocator_type alloc)
    : _name(alloc) {}

Attribute::Attribute(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , AttributeDefinition::EObjectType object_type
    , Attribute::value_t value)

    : _name(std::move(name), alloc)
    , _object_type(object_type)
    , _value(std::move(value))
    , _allocator(alloc)
{}
Attribute Attribute::Clone() const
{
    return {*this, _allocator};
}
const std::string_view Attribute::Name() const
{
    return _name;
}
AttributeDefinition::EObjectType Attribute::ObjectType() const
{
    return _object_type;
};
const Attribute::value_t& Attribute::Value() const
{
    return _value;
}
bool Attribute::operator==(const Attribute& rhs) const
{
    bool result = true;
    result &= _name == rhs.Name();
    result &= _object_type == rhs.ObjectType();
    result &= _value == rhs.Value();
    return result;
}
bool Attribute::operator!=(const Attribute& rhs) const
{
    return !(*this == rhs);
}
