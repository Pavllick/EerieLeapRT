
#include "dbcppp/ValueEncodingDescriptionImpl.h"

using namespace dbcppp;

pmr_unique_ptr<IValueEncodingDescription> IValueEncodingDescription::Create(
    std::pmr::memory_resource* mr
    , int64_t value
    , std::pmr::string&& description)
{
    return make_unique_pmr<ValueEncodingDescriptionImpl>(
          mr
        , value
        , std::move(description));
}

ValueEncodingDescriptionImpl::ValueEncodingDescriptionImpl(
      std::allocator_arg_t
    , allocator_type alloc
    , int64_t value
    , std::pmr::string&& description)
    : _value(value)
    , _description(std::move(description))
    , _allocator(alloc)
{}
pmr_unique_ptr<IValueEncodingDescription> ValueEncodingDescriptionImpl::Clone() const
{
    return make_unique_pmr<ValueEncodingDescriptionImpl>(_allocator, *this);
}
int64_t ValueEncodingDescriptionImpl::Value() const
{
    return _value;
}
const std::string_view ValueEncodingDescriptionImpl::Description() const
{
    return _description;
}

bool ValueEncodingDescriptionImpl::operator==(const IValueEncodingDescription& rhs) const
{
    bool equal = true;
    equal &= _value == rhs.Value();
    equal &= _description == rhs.Description();
    return equal;
}
bool ValueEncodingDescriptionImpl::operator!=(const IValueEncodingDescription& rhs) const
{
    return !(*this == rhs);
}
