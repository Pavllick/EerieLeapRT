#include <algorithm>
#include <memory>
#include "dbcppp/EnvironmentVariable.h"

using namespace dbcppp;

EnvironmentVariable::EnvironmentVariable(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , EVarType var_type
    , double minimum
    , double maximum
    , std::pmr::string&& unit
    , double initial_value
    , uint64_t ev_id
    , EAccessType access_type
    , std::pmr::vector<std::pmr::string>&& access_nodes
    , std::pmr::vector<ValueEncodingDescription>&& value_encoding_descriptions
    , uint64_t data_size
    , std::pmr::vector<Attribute>&& attribute_values
    , std::pmr::string&& comment)

    : _name(std::move(name), alloc)
    , _var_type(std::move(var_type))
    , _minimum(std::move(minimum))
    , _maximum(std::move(maximum))
    , _unit(std::move(unit), alloc)
    , _initial_value(std::move(initial_value))
    , _ev_id(std::move(ev_id))
    , _access_type(std::move(access_type))
    , _access_nodes(std::move(access_nodes), alloc)
    , _value_encoding_descriptions(std::move(value_encoding_descriptions), alloc)
    , _data_size(std::move(data_size))
    , _attribute_values(std::move(attribute_values), alloc)
    , _comment(std::move(comment), alloc)
    , _allocator(alloc)
{}
EnvironmentVariable EnvironmentVariable::Clone() const
{
    return {*this, _allocator};
}
const std::string_view EnvironmentVariable::Name() const
{
    return _name;
}
EnvironmentVariable::EVarType EnvironmentVariable::VarType() const
{
    return _var_type;
}
double EnvironmentVariable::Minimum() const
{
    return _minimum;
}
double EnvironmentVariable::Maximum() const
{
    return _maximum;
}
const std::string_view EnvironmentVariable::Unit() const
{
    return _unit;
}
double EnvironmentVariable::InitialValue() const
{
    return _initial_value;
}
uint64_t EnvironmentVariable::EvId() const
{
    return _ev_id;
}
EnvironmentVariable::EAccessType EnvironmentVariable::AccessType() const
{
    return _access_type;
}
const std::pmr::string& EnvironmentVariable::AccessNodes_Get(std::size_t i) const
{
    return _access_nodes[i];
}
std::size_t EnvironmentVariable::AccessNodes_Size() const
{
    return _access_nodes.size();
}
const ValueEncodingDescription& EnvironmentVariable::ValueEncodingDescriptions_Get(std::size_t i) const
{
    return _value_encoding_descriptions[i];
}
std::size_t EnvironmentVariable::ValueEncodingDescriptions_Size() const
{
    return _value_encoding_descriptions.size();
}
uint64_t EnvironmentVariable::DataSize() const
{
    return _data_size;
}
const Attribute& EnvironmentVariable::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
std::size_t EnvironmentVariable::AttributeValues_Size() const
{
    return _attribute_values.size();
}
const std::string_view EnvironmentVariable::Comment() const
{
    return _comment;
}
bool EnvironmentVariable::operator==(const EnvironmentVariable& rhs) const
{
    bool result = true;
    result &= _name == rhs.Name();
    result &= _var_type == rhs.VarType();
    result &= _minimum == rhs.Minimum();
    result &= _unit == rhs.Unit();
    result &= _initial_value == rhs.InitialValue();
    result &= _ev_id == rhs.EvId();
    result &= _access_type == rhs.AccessType();
    for (const auto& node : rhs.AccessNodes())
    {
        auto beg = _access_nodes.begin();
        auto end = _access_nodes.end();
        result &= std::ranges::find(beg, end, node) != end;
    }
    for (const auto& ved : rhs.ValueEncodingDescriptions())
    {
        auto beg = _value_encoding_descriptions.begin();
        auto end = _value_encoding_descriptions.end();
        result &= std::ranges::find(beg, end, ved) != end;
    }
    result &= _data_size == rhs.DataSize();
    for (const auto& attr : rhs.AttributeValues())
    {
        auto beg = _attribute_values.begin();
        auto end = _attribute_values.end();
        result &= std::ranges::find(beg, end, attr) != end;
    }
    result &= _comment == rhs.Comment();
    return result;
}
bool EnvironmentVariable::operator!=(const EnvironmentVariable& rhs) const
{
    return !(*this == rhs);
}
