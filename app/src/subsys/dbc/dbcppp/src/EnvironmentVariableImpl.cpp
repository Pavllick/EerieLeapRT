#include <algorithm>
#include "EnvironmentVariableImpl.h"

using namespace dbcppp;

std::unique_ptr<IEnvironmentVariable> IEnvironmentVariable::Create(
      std::pmr::memory_resource* mr
    , std::pmr::string&& name
    , EVarType var_type
    , double minimum
    , double maximum
    , std::pmr::string&& unit
    , double initial_value
    , uint64_t ev_id
    , EAccessType access_type
    , std::pmr::vector<std::pmr::string>&& access_nodes
    , std::pmr::vector<std::unique_ptr<IValueEncodingDescription>>&& value_encoding_descriptions
    , uint64_t data_size
    , std::pmr::vector<std::unique_ptr<IAttribute>>&& attribute_values
    , std::pmr::string&& comment)
{
    std::pmr::vector<AttributeImpl> avs(mr);
    avs.reserve(attribute_values.size());
    for (auto& av : attribute_values)
    {
        avs.push_back(std::move(static_cast<AttributeImpl&>(*av)));
        av.reset(nullptr);
    }
    std::pmr::vector<ValueEncodingDescriptionImpl> veds(mr);
    veds.reserve(value_encoding_descriptions.size());
    for (auto& ved : value_encoding_descriptions)
    {
        veds.push_back(std::move(static_cast<ValueEncodingDescriptionImpl&>(*ved)));
        ved.reset(nullptr);
    }
    return std::make_unique<EnvironmentVariableImpl>(
          std::allocator_arg
        , mr
        , std::move(name)
        , var_type
        , minimum
        , maximum
        , std::move(unit)
        , initial_value
        , ev_id
        , access_type
        , std::move(access_nodes)
        , std::move(veds)
        , data_size
        , std::move(avs)
        , std::move(comment));
}

EnvironmentVariableImpl::EnvironmentVariableImpl(
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
    , std::pmr::vector<ValueEncodingDescriptionImpl>&& value_encoding_descriptions
    , uint64_t data_size
    , std::pmr::vector<AttributeImpl>&& attribute_values
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
std::unique_ptr<IEnvironmentVariable> EnvironmentVariableImpl::Clone() const
{
    return std::make_unique<EnvironmentVariableImpl>(*this, _allocator);
}
const std::string_view EnvironmentVariableImpl::Name() const
{
    return _name;
}
IEnvironmentVariable::EVarType EnvironmentVariableImpl::VarType() const
{
    return _var_type;
}
double EnvironmentVariableImpl::Minimum() const
{
    return _minimum;
}
double EnvironmentVariableImpl::Maximum() const
{
    return _maximum;
}
const std::string_view EnvironmentVariableImpl::Unit() const
{
    return _unit;
}
double EnvironmentVariableImpl::InitialValue() const
{
    return _initial_value;
}
uint64_t EnvironmentVariableImpl::EvId() const
{
    return _ev_id;
}
IEnvironmentVariable::EAccessType EnvironmentVariableImpl::AccessType() const
{
    return _access_type;
}
const std::pmr::string& EnvironmentVariableImpl::AccessNodes_Get(std::size_t i) const
{
    return _access_nodes[i];
}
uint64_t EnvironmentVariableImpl::AccessNodes_Size() const
{
    return _access_nodes.size();
}
const IValueEncodingDescription& EnvironmentVariableImpl::ValueEncodingDescriptions_Get(std::size_t i) const
{
    return _value_encoding_descriptions[i];
}
uint64_t EnvironmentVariableImpl::ValueEncodingDescriptions_Size() const
{
    return _value_encoding_descriptions.size();
}
uint64_t EnvironmentVariableImpl::DataSize() const
{
    return _data_size;
}
const IAttribute& EnvironmentVariableImpl::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
uint64_t EnvironmentVariableImpl::AttributeValues_Size() const
{
    return _attribute_values.size();
}
const std::string_view EnvironmentVariableImpl::Comment() const
{
    return _comment;
}
bool EnvironmentVariableImpl::operator==(const IEnvironmentVariable& rhs) const
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
bool EnvironmentVariableImpl::operator!=(const IEnvironmentVariable& rhs) const
{
    return !(*this == rhs);
}
