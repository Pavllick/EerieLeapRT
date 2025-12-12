#pragma once

#include <vector>
#include <memory>

#include "dbcppp/Node.h"
#include "AttributeImpl.h"

namespace dbcppp
{
    class NodeImpl final
        : public INode
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        NodeImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , std::pmr::string&& comment
            , std::pmr::vector<AttributeImpl>&& attribute_values);

        NodeImpl& operator=(const NodeImpl&) noexcept = default;
        NodeImpl& operator=(NodeImpl&&) noexcept = default;
        NodeImpl(NodeImpl&&) noexcept = default;
        ~NodeImpl() = default;

        NodeImpl(NodeImpl&& other, allocator_type alloc)
            : _name(std::move(other._name))
            , _comment(std::move(other._comment))
            , _attribute_values(std::move(other._attribute_values))
            , _allocator(alloc) {}

        NodeImpl(const NodeImpl& other, allocator_type alloc = {})
            : _name(other._name, alloc)
            , _comment(other._comment, alloc)
            , _attribute_values(other._attribute_values, alloc)
            , _allocator(alloc) {}

        virtual std::unique_ptr<INode> Clone() const override;

        virtual const std::string_view Name() const override;
        virtual const IAttribute& AttributeValues_Get(std::size_t i) const override;
        virtual uint64_t AttributeValues_Size() const override;
        virtual const std::string_view Comment() const override;

        virtual bool operator==(const INode& rhs) const override;
        virtual bool operator!=(const INode& rhs) const override;

    private:
        std::pmr::string _name;
        std::pmr::string _comment;
        std::pmr::vector<AttributeImpl> _attribute_values;

        allocator_type _allocator;
    };
}
