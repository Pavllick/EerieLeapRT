#include <algorithm>
#include "AttributeDefinitionImpl.h"

using namespace dbcppp;

pmr_unique_ptr<IAttributeDefinition> IAttributeDefinition::Create(
      std::pmr::memory_resource* mr
    , std::pmr::string&& name
    , EObjectType object_type
    , value_type_t&& value_type)
{
    return make_unique_pmr<AttributeDefinitionImpl>(
          mr
        , std::move(name)
        , object_type
        , std::move(value_type));
}

AttributeDefinitionImpl::AttributeDefinitionImpl(
    std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , EObjectType object_type
    , value_type_t value_type)

    : _name(std::move(name), alloc)
    , _object_type(std::move(object_type))
    , _value_type(std::move(value_type))
    , _allocator(alloc)
{}
pmr_unique_ptr<IAttributeDefinition> AttributeDefinitionImpl::Clone() const
{
    return make_unique_pmr<AttributeDefinitionImpl>(_allocator, *this);
}
IAttributeDefinition::EObjectType AttributeDefinitionImpl::ObjectType() const
{
    return _object_type;
}
const std::string_view AttributeDefinitionImpl::Name() const
{
    return _name;
}
const IAttributeDefinition::value_type_t& AttributeDefinitionImpl::ValueType() const
{
    return _value_type;
}
bool AttributeDefinitionImpl::operator==(const IAttributeDefinition& rhs) const
{
    bool equal = true;
    equal &= _name == rhs.Name();
    equal &= _object_type == rhs.ObjectType();
    equal &= _value_type.index() == rhs.ValueType().index();
    if (equal)
    {
        auto cmp =
            [](const auto& lhs, const auto& rhs)
            {
                return lhs.minimum == rhs.minimum && lhs.maximum == rhs.maximum;
            };
        if (std::get_if<ValueTypeInt>(&_value_type))
        {
            equal &= cmp(std::get<ValueTypeInt>(_value_type), std::get<ValueTypeInt>(rhs.ValueType()));
        }
        else if (std::get_if<ValueTypeHex>(&_value_type))
        {
            equal &= cmp(std::get<ValueTypeHex>(_value_type), std::get<ValueTypeHex>(rhs.ValueType()));
        }
        else if (std::get_if<ValueTypeFloat>(&_value_type))
        {
            equal &= cmp(std::get<ValueTypeFloat>(_value_type), std::get<ValueTypeFloat>(rhs.ValueType()));
        }
        else if (std::get_if<ValueTypeString>(&_value_type))
        {
            equal &= true;
        }
        else if (std::get_if<ValueTypeEnum>(&_value_type))
        {
            const auto& lhs_ = std::get<ValueTypeEnum>(_value_type);
            const auto& rhs_ = std::get<ValueTypeEnum>(rhs.ValueType());
            for (const auto& v : lhs_.values)
            {
                auto beg = rhs_.values.begin();
                auto end = rhs_.values.end();
                equal &= std::ranges::find(beg, end, v) != end;
            }
        }
    }
    return equal;
}
bool AttributeDefinitionImpl::operator!=(const IAttributeDefinition& rhs) const
{
    return !(*this == rhs);
}
