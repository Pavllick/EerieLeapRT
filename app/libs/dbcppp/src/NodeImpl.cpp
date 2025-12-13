#include <algorithm>
#include "dbcppp/Node.h"

using namespace dbcppp;

Node::Node(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , std::pmr::string&& comment
    , std::pmr::vector<Attribute>&& attribute_values)

    : _name(std::move(name), alloc)
    , _comment(std::move(comment), alloc)
    , _attribute_values(std::move(attribute_values), alloc)
    , _allocator(alloc)
{}
Node Node::Clone() const
{
    return {*this, _allocator};
}
const std::string_view Node::Name() const
{
    return _name;
}
const std::string_view Node::Comment() const
{
    return _comment;
}
const Attribute& Node::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
std::size_t Node::AttributeValues_Size() const
{
    return _attribute_values.size();
}
bool Node::operator==(const Node& rhs) const
{
    if (_name != rhs._name) return false;
    if (_comment != rhs._comment) return false;
    if (_attribute_values.size() != rhs._attribute_values.size()) return false;

    return std::ranges::is_permutation(
        _attribute_values.begin(),
        _attribute_values.end(),
        rhs._attribute_values.begin(),
        rhs._attribute_values.end());
}
bool Node::operator!=(const Node& rhs) const
{
    return !(*this == rhs);
}
