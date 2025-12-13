#include <algorithm>
#include "dbcppp/Node.h"

using namespace dbcppp;

Node Node::Create(
    std::pmr::memory_resource* mr,
    std::pmr::string&& name,
    std::pmr::string&& comment,
    std::pmr::vector<Attribute>&& attribute_values)
{

    return {std::allocator_arg, mr, std::move(name), std::move(comment), std::move(attribute_values)};
}
Node::Node(
      std::allocator_arg_t
    , allocator_type alloc
    , std::pmr::string&& name
    , std::pmr::string&& comment
    , std::pmr::vector<Attribute>&& attribute_values)

    : _name(std::move(name))
    , _comment(std::move(comment))
    , _attribute_values(std::move(attribute_values))
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
bool Node::operator!=(const Node& rhs) const
{
    return !(*this == rhs);
}
