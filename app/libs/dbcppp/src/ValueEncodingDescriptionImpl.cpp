
#include "dbcppp/ValueEncodingDescription.h"
#include <memory>

using namespace dbcppp;

ValueEncodingDescription ValueEncodingDescription::Create(
    std::pmr::memory_resource* mr
    , int64_t value
    , std::pmr::string&& description)
{
    return {
          std::allocator_arg
        , mr
        , value
        , std::move(description)
    };
}

ValueEncodingDescription::ValueEncodingDescription(
      std::allocator_arg_t
    , allocator_type alloc
    , int64_t value
    , std::pmr::string&& description)
    : _value(value)
    , _description(std::move(description))
    , _allocator(alloc)
{}
ValueEncodingDescription ValueEncodingDescription::Clone() const
{
    return {*this, _allocator};
}
int64_t ValueEncodingDescription::Value() const
{
    return _value;
}
const std::string_view ValueEncodingDescription::Description() const
{
    return _description;
}

bool ValueEncodingDescription::operator==(const ValueEncodingDescription& rhs) const
{
    bool equal = true;
    equal &= _value == rhs.Value();
    equal &= _description == rhs.Description();
    return equal;
}
bool ValueEncodingDescription::operator!=(const ValueEncodingDescription& rhs) const
{
    return !(*this == rhs);
}
