#include <algorithm>
#include "dbcppp/ValueTableImpl.h"

using namespace dbcppp;

pmr_unique_ptr<IValueTable> IValueTable::Create(
      std::pmr::memory_resource* mr
    , std::pmr::string&& name
    , std::optional<pmr_unique_ptr<ISignalType>>&& signal_type
    , std::pmr::vector<pmr_unique_ptr<IValueEncodingDescription>>&& value_encoding_descriptions)
{
    std::pmr::vector<ValueEncodingDescriptionImpl> veds(mr);
    veds.reserve(value_encoding_descriptions.size());
    for (auto& ved : value_encoding_descriptions)
    {
        veds.push_back(std::move(static_cast<ValueEncodingDescriptionImpl&>(*ved)));
        ved.reset(nullptr);
    }

    if (signal_type)
    {
        return make_unique_pmr<ValueTableImpl>(
              mr
            , std::move(name)
            , std::move(static_cast<SignalTypeImpl&>(*signal_type.value()))
            , std::move(veds));
    }
    return make_unique_pmr<ValueTableImpl>(
        mr
        , std::move(name)
        , std::nullopt
        , std::move(veds));
}
ValueTableImpl::ValueTableImpl(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , std::optional<SignalTypeImpl>&& signal_type
    , std::pmr::vector<ValueEncodingDescriptionImpl>&& value_encoding_descriptions)

    : _name(std::move(name))
    , _signal_type(signal_type)
    , _value_encoding_descriptions(std::move(value_encoding_descriptions))
    , _allocator(alloc)
{}
pmr_unique_ptr<IValueTable> ValueTableImpl::Clone() const
{
    return make_unique_pmr<ValueTableImpl>(_allocator, *this);
}
const std::string_view ValueTableImpl::Name() const
{
    return _name;
}
std::optional<std::reference_wrapper<const ISignalType>> ValueTableImpl::SignalType() const
{
    std::optional<std::reference_wrapper<const ISignalType>> signal_type;
    if (_signal_type)
    {
        signal_type = *_signal_type;
    }
    return signal_type;
}
const IValueEncodingDescription& ValueTableImpl::ValueEncodingDescriptions_Get(std::size_t i) const
{
    return _value_encoding_descriptions[i];
}
std::size_t ValueTableImpl::ValueEncodingDescriptions_Size() const
{
    return _value_encoding_descriptions.size();
}
bool ValueTableImpl::operator==(const IValueTable& rhs) const
{
    bool equal = true;
    equal &= _name == rhs.Name();
    equal &= _signal_type == rhs.SignalType();
    for (const auto& ved : rhs.ValueEncodingDescriptions())
    {
        auto beg = _value_encoding_descriptions.begin();
        auto end = _value_encoding_descriptions.end();
        equal &= std::ranges::find(beg, end, ved) != end;
    }
    return equal;
}
bool ValueTableImpl::operator!=(const IValueTable& rhs) const
{
    return !(*this == rhs);
}
