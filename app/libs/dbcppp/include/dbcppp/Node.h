#pragma once

#include <vector>
#include <memory>

#include "Iterator.h"
#include "Attribute.h"

namespace dbcppp
{
    class Node final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        Node(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , std::pmr::string&& comment
            , std::pmr::vector<Attribute>&& attribute_values);

        Node& operator=(const Node&) noexcept = default;
        Node& operator=(Node&&) noexcept = default;
        Node(Node&&) noexcept = default;
        ~Node() = default;

        Node(Node&& other, allocator_type alloc)
            : _name(std::move(other._name), alloc)
            , _comment(std::move(other._comment), alloc)
            , _attribute_values(std::move(other._attribute_values), alloc)
            , _allocator(alloc) {}

        Node(const Node& other, allocator_type alloc = {})
            : _name(other._name, alloc)
            , _comment(other._comment, alloc)
            , _attribute_values(other._attribute_values, alloc)
            , _allocator(alloc) {}

        Node Clone() const;

        const std::string_view Name() const;
        const Attribute& AttributeValues_Get(std::size_t i) const;
        std::size_t AttributeValues_Size() const;
        const std::string_view Comment() const;

        bool operator==(const Node& rhs) const;
        bool operator!=(const Node& rhs) const;

        DBCPPP_MAKE_ITERABLE(Node, AttributeValues, Attribute);

    private:
        std::pmr::string _name;
        std::pmr::string _comment;
        std::pmr::vector<Attribute> _attribute_values;

        allocator_type _allocator;
    };
}
