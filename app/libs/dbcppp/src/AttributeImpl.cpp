#include "AttributeImpl.h"
#include "dbcppp/Network.h"

using namespace dbcppp;

pmr_unique_ptr<IAttribute> IAttribute::Create(
      std::pmr::memory_resource* mr
    , std::pmr::string&& name
    , IAttributeDefinition::EObjectType object_type
    , value_t value)
{
    return make_unique_pmr<AttributeImpl>(
          mr
        , std::move(name)
        , object_type
        , std::move(value));
}

AttributeImpl::AttributeImpl(std::allocator_arg_t, allocator_type alloc)
    : _name(alloc) {}

AttributeImpl::AttributeImpl(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , IAttributeDefinition::EObjectType object_type
    , IAttribute::value_t value)

    : _name(std::move(name), alloc)
    , _object_type(object_type)
    , _value(std::move(value))
    , _allocator(alloc)
{}
pmr_unique_ptr<IAttribute> AttributeImpl::Clone() const
{
    return make_unique_pmr<AttributeImpl>(_allocator, *this);
}
const std::string_view AttributeImpl::Name() const
{
    return _name;
}
IAttributeDefinition::EObjectType AttributeImpl::ObjectType() const
{
    return _object_type;
};
const IAttribute::value_t& AttributeImpl::Value() const
{
    return _value;
}
bool AttributeImpl::operator==(const IAttribute& rhs) const
{
    bool result = true;
    result &= _name == rhs.Name();
    result &= _object_type == rhs.ObjectType();
    result &= _value == rhs.Value();
    return result;
}
bool AttributeImpl::operator!=(const IAttribute& rhs) const
{
    return !(*this == rhs);
}
