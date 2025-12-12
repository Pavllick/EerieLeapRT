#pragma once

#include "dbcppp/Message.h"
#include "dbcppp/Network.h"
#include "dbcppp/BitTimingImpl.h"
#include "dbcppp/ValueTableImpl.h"
#include "dbcppp/MessageImpl.h"
#include "dbcppp/EnvironmentVariableImpl.h"
#include "dbcppp/AttributeDefinitionImpl.h"
#include "dbcppp/AttributeImpl.h"

namespace dbcppp
{
    class NetworkImpl final
        : public INetwork
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        NetworkImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& version
            , std::pmr::vector<std::pmr::string>&& new_symbols
            , BitTimingImpl&& bit_timing
            , std::pmr::vector<NodeImpl>&& nodes
            , std::pmr::vector<ValueTableImpl>&& value_tables
            , std::pmr::vector<MessageImpl>&& messages
            , std::pmr::vector<EnvironmentVariableImpl>&& environment_variables
            , std::pmr::vector<AttributeDefinitionImpl>&& attribute_definitions
            , std::pmr::vector<AttributeImpl>&& attribute_defaults
            , std::pmr::vector<AttributeImpl>&& attribute_values
            , std::pmr::string&& comment);

        NetworkImpl(const NetworkImpl&) = delete;
        NetworkImpl& operator=(const NetworkImpl&) noexcept = default;
        NetworkImpl& operator=(NetworkImpl&&) noexcept = default;
        NetworkImpl(NetworkImpl &&) = default;
        ~NetworkImpl() = default;

        NetworkImpl(NetworkImpl &&other, allocator_type alloc)
            : _version(std::move(other._version), alloc)
            , _new_symbols(std::move(other._new_symbols), alloc)
            , _bit_timing(other._bit_timing)
            , _nodes(std::move(other._nodes), alloc)
            , _value_tables(std::move(other._value_tables), alloc)
            , _messages(std::move(other._messages), alloc)
            , _environment_variables(std::move(other._environment_variables), alloc)
            , _attribute_definitions(std::move(other._attribute_definitions), alloc)
            , _attribute_defaults(std::move(other._attribute_defaults), alloc)
            , _attribute_values(std::move(other._attribute_values), alloc)
            , _comment(std::move(other._comment), alloc)
            , _allocator(alloc)
            {}

        NetworkImpl(const NetworkImpl &other, allocator_type alloc = {})
            : _version(other._version, alloc)
            , _new_symbols(other._new_symbols, alloc)
            , _bit_timing(other._bit_timing)
            , _nodes(other._nodes, alloc)
            , _value_tables(other._value_tables, alloc)
            , _messages(other._messages, alloc)
            , _environment_variables(other._environment_variables, alloc)
            , _attribute_definitions(other._attribute_definitions, alloc)
            , _attribute_defaults(other._attribute_defaults, alloc)
            , _attribute_values(other._attribute_values, alloc)
            , _comment(other._comment, alloc)
            , _allocator(alloc)
            {}

        pmr_unique_ptr<INetwork> Clone() const override;

        const std::string_view Version() const override;
        const std::pmr::string& NewSymbols_Get(std::size_t i) const override;
        std::size_t NewSymbols_Size() const override;
        const IBitTiming& BitTiming() const override;
        const INode& Nodes_Get(std::size_t i) const override;
        std::size_t Nodes_Size() const override;
        const IValueTable& ValueTables_Get(std::size_t i) const override;
        std::size_t ValueTables_Size() const override;
        const IMessage& Messages_Get(std::size_t i) const override;
        std::size_t Messages_Size() const override;
        const IEnvironmentVariable& EnvironmentVariables_Get(std::size_t i) const override;
        std::size_t EnvironmentVariables_Size() const override;
        const IAttributeDefinition& AttributeDefinitions_Get(std::size_t i) const override;
        std::size_t AttributeDefinitions_Size() const override;
        const IAttribute& AttributeDefaults_Get(std::size_t i) const override;
        std::size_t AttributeDefaults_Size() const override;
        const IAttribute& AttributeValues_Get(std::size_t i) const override;
        std::size_t AttributeValues_Size() const override;
        const std::string_view Comment() const override;

        const IMessage* ParentMessage(const ISignal* sig) const override;

        bool operator==(const INetwork& rhs) const override;
        bool operator!=(const INetwork& rhs) const override;

        std::string_view version();
        std::pmr::vector<std::pmr::string>& newSymbols();
        BitTimingImpl& bitTiming();
        std::pmr::vector<NodeImpl>& nodes();
        std::pmr::vector<ValueTableImpl>& valueTables();
        std::pmr::vector<MessageImpl>& messages();
        std::pmr::vector<EnvironmentVariableImpl>& environmentVariables();
        std::pmr::vector<AttributeDefinitionImpl>& attributeDefinitions();
        std::pmr::vector<AttributeImpl>& attributeDefaults();
        std::pmr::vector<AttributeImpl>& attributeValues();
        std::string_view comment();

    private:
        std::pmr::string _version;
        std::pmr::vector<std::pmr::string> _new_symbols;
        BitTimingImpl _bit_timing;
        std::pmr::vector<NodeImpl> _nodes;
        std::pmr::vector<ValueTableImpl> _value_tables;
        std::pmr::vector<MessageImpl> _messages;
        std::pmr::vector<EnvironmentVariableImpl> _environment_variables;
        std::pmr::vector<AttributeDefinitionImpl> _attribute_definitions;
        std::pmr::vector<AttributeImpl> _attribute_defaults;
        std::pmr::vector<AttributeImpl> _attribute_values;
        std::pmr::string _comment;

        allocator_type _allocator;
    };
}
