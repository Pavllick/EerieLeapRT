#include <algorithm>
#include "dbcppp/NodeImpl.h"

using namespace dbcppp;

pmr_unique_ptr<INode> INode::Create(
    std::pmr::memory_resource* mr,
    std::pmr::string&& name,
    std::pmr::string&& comment,
    std::pmr::vector<pmr_unique_ptr<IAttribute>>&& attribute_values)
{
    std::pmr::vector<AttributeImpl> avs(mr);
    for (auto& av : attribute_values)
    {
        avs.push_back(std::move(static_cast<AttributeImpl&>(*av)));
        av.reset(nullptr);
    }
    return make_unique_pmr<NodeImpl>(mr, std::move(name), std::move(comment), std::move(avs));
}
NodeImpl::NodeImpl(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , std::pmr::string&& comment
    , std::pmr::vector<AttributeImpl>&& attribute_values)

    : _name(name, alloc)
    , _comment(comment, alloc)
    , _attribute_values(std::move(attribute_values), alloc)
    , _allocator(alloc)
{}
pmr_unique_ptr<INode> NodeImpl::Clone() const
{
    return make_unique_pmr<NodeImpl>(_allocator, *this);
}
const std::string_view NodeImpl::Name() const
{
    return _name;
}
const std::string_view NodeImpl::Comment() const
{
    return _comment;
}
const IAttribute& NodeImpl::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
std::size_t NodeImpl::AttributeValues_Size() const
{
    return _attribute_values.size();
}
bool NodeImpl::operator==(const INode& rhs) const
{
    bool equal = true;
    equal &= _name == rhs.Name();
    equal &= _comment == rhs.Comment();
    for (const auto& attr : rhs.AttributeValues())
    {
        auto beg = _attribute_values.begin();
        auto end = _attribute_values.end();
        equal &= std::ranges::find(beg, end, attr) != end;
    }
    return equal;
}
bool NodeImpl::operator!=(const INode& rhs) const
{
    return !(*this == rhs);
}
