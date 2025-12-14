#pragma once

#include <memory_resource>
#include <string>
#include <vector>
#include <istream>

#include "Iterator.h"
#include "BitTiming.h"
#include "ValueTable.h"
#include "Node.h"
#include "Message.h"
#include "EnvironmentVariable.h"
#include "AttributeDefinition.h"
#include "Attribute.h"

namespace dbcppp
{
    class Network final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        Network(
              std::allocator_arg_t
            , allocator_type alloc);

        Network(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& version
            , std::pmr::vector<std::pmr::string>&& new_symbols
            , BitTiming&& bit_timing
            , std::pmr::vector<Node>&& nodes
            , std::pmr::vector<ValueTable>&& value_tables
            , std::pmr::vector<Message>&& messages
            , std::pmr::vector<EnvironmentVariable>&& environment_variables
            , std::pmr::vector<AttributeDefinition>&& attribute_definitions
            , std::pmr::vector<Attribute>&& attribute_defaults
            , std::pmr::vector<Attribute>&& attribute_values
            , std::pmr::string&& comment);

        Network(const Network&) = delete;
        Network& operator=(const Network&) noexcept = default;
        Network& operator=(Network&&) noexcept = default;
        Network(Network &&) = default;
        ~Network() = default;

        Network(Network &&other, allocator_type alloc)
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

        Network(const Network &other, allocator_type alloc = {})
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

        static std::shared_ptr<Network> LoadDBCFromIs(std::pmr::memory_resource* mr, std::istream &is);

        Network Clone() const;

        void Merge(Network&& other);

        const std::string_view Version() const;
        const std::pmr::string& NewSymbols_Get(std::size_t i) const;
        std::size_t NewSymbols_Size() const;
        const BitTiming& GetBitTiming() const;
        const Node& Nodes_Get(std::size_t i) const;
        std::size_t Nodes_Size() const;
        const ValueTable& ValueTables_Get(std::size_t i) const;
        std::size_t ValueTables_Size() const;
        const Message& Messages_Get(std::size_t i) const;
        std::size_t Messages_Size() const;
        const EnvironmentVariable& EnvironmentVariables_Get(std::size_t i) const;
        std::size_t EnvironmentVariables_Size() const;
        const AttributeDefinition& AttributeDefinitions_Get(std::size_t i) const;
        std::size_t AttributeDefinitions_Size() const;
        const Attribute& AttributeDefaults_Get(std::size_t i) const;
        std::size_t AttributeDefaults_Size() const;
        const Attribute& AttributeValues_Get(std::size_t i) const;
        std::size_t AttributeValues_Size() const;
        const std::string_view Comment() const;

        const Message* ParentMessage(const Signal* sig) const;

        bool operator==(const Network& rhs) const;
        bool operator!=(const Network& rhs) const;

        DBCPPP_MAKE_ITERABLE(Network, NewSymbols, std::pmr::string);
        DBCPPP_MAKE_ITERABLE(Network, Nodes, Node);
        DBCPPP_MAKE_ITERABLE(Network, ValueTables, ValueTable);
        DBCPPP_MAKE_ITERABLE(Network, Messages, Message);
        DBCPPP_MAKE_ITERABLE(Network, EnvironmentVariables, EnvironmentVariable);
        DBCPPP_MAKE_ITERABLE(Network, AttributeDefinitions, AttributeDefinition);
        DBCPPP_MAKE_ITERABLE(Network, AttributeDefaults, Attribute);
        DBCPPP_MAKE_ITERABLE(Network, AttributeValues, Attribute);

        void AddMessage(Message&& msg);

        std::pmr::vector<Message>& GetMessages() {
            return _messages;
        }

    private:
        std::pmr::string _version;
        std::pmr::vector<std::pmr::string> _new_symbols;
        BitTiming _bit_timing;
        std::pmr::vector<Node> _nodes;
        std::pmr::vector<ValueTable> _value_tables;
        std::pmr::vector<Message> _messages;
        std::pmr::vector<EnvironmentVariable> _environment_variables;
        std::pmr::vector<AttributeDefinition> _attribute_definitions;
        std::pmr::vector<Attribute> _attribute_defaults;
        std::pmr::vector<Attribute> _attribute_values;
        std::pmr::string _comment;

        allocator_type _allocator;

        std::string_view version();
        std::pmr::vector<std::pmr::string>& newSymbols();
        BitTiming& bitTiming();
        std::pmr::vector<Node>& nodes();
        std::pmr::vector<ValueTable>& valueTables();
        std::pmr::vector<Message>& messages();
        std::pmr::vector<EnvironmentVariable>& environmentVariables();
        std::pmr::vector<AttributeDefinition>& attributeDefinitions();
        std::pmr::vector<Attribute>& attributeDefaults();
        std::pmr::vector<Attribute>& attributeValues();
        std::string_view comment();
    };
}
