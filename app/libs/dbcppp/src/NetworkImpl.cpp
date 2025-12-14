#include <algorithm>
#include <memory>
#include "dbcppp/Network.h"
#include "dbcppp/Signal.h"
#include "dbcppp/Network.h"

using namespace dbcppp;

Network::Network(
        std::allocator_arg_t
    , allocator_type alloc)

    : _version(alloc)
    , _new_symbols(alloc)
    , _nodes(alloc)
    , _value_tables(alloc)
    , _messages(alloc)
    , _environment_variables(alloc)
    , _attribute_definitions(alloc)
    , _attribute_defaults(alloc)
    , _attribute_values(alloc)
    , _comment(alloc)
    , _allocator(alloc)
{}

Network::Network(
      std::allocator_arg_t alloc_arg
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
    , std::pmr::string&& comment)

    : _version(std::move(version), alloc)
    , _new_symbols(std::move(new_symbols), alloc)
    , _bit_timing(bit_timing)
    , _nodes(std::move(nodes), alloc)
    , _value_tables(std::move(value_tables), alloc)
    , _messages(std::move(messages), alloc)
    , _environment_variables(std::move(environment_variables), alloc)
    , _attribute_definitions(std::move(attribute_definitions), alloc)
    , _attribute_defaults(std::move(attribute_defaults), alloc)
    , _attribute_values(std::move(attribute_values), alloc)
    , _comment(std::move(comment), alloc)
    , _allocator(alloc)
{}
Network Network::Clone() const
{
    return {*this, _allocator};
}
const std::string_view Network::Version() const
{
    return _version;
}
const std::pmr::string& Network::NewSymbols_Get(std::size_t i) const
{
    return _new_symbols[i];
}
std::size_t Network::NewSymbols_Size() const
{
    return _new_symbols.size();
}
const BitTiming& Network::GetBitTiming() const
{
    return _bit_timing;
}
const Node& Network::Nodes_Get(std::size_t i) const
{
    return _nodes[i];
}
std::size_t Network::Nodes_Size() const
{
    return _nodes.size();
}
const ValueTable& Network::ValueTables_Get(std::size_t i) const
{
    return _value_tables[i];
}
std::size_t Network::ValueTables_Size() const
{
    return _value_tables.size();
}
const Message& Network::Messages_Get(std::size_t i) const
{
    return _messages[i];
}
std::size_t Network::Messages_Size() const
{
    return _messages.size();
}
const EnvironmentVariable& Network::EnvironmentVariables_Get(std::size_t i) const
{
    return _environment_variables[i];
}
std::size_t Network::EnvironmentVariables_Size() const
{
    return _environment_variables.size();
}
const AttributeDefinition& Network::AttributeDefinitions_Get(std::size_t i) const
{
    return _attribute_definitions[i];
}
std::size_t Network::AttributeDefinitions_Size() const
{
    return _attribute_definitions.size();
}
const Attribute& Network::AttributeDefaults_Get(std::size_t i) const
{
    return _attribute_defaults[i];
}
std::size_t Network::AttributeDefaults_Size() const
{
    return _attribute_defaults.size();
}
const Attribute& Network::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
std::size_t Network::AttributeValues_Size() const
{
    return _attribute_values.size();
}
const std::string_view Network::Comment() const
{
    return _comment;
}
const Message* Network::ParentMessage(const Signal* sig) const
{
    const Message* parent = nullptr;
    for (const auto& msg : _messages)
    {
        auto iter = std::find_if(msg.signals().begin(), msg.signals().end(),
            [&](const Signal& other) { return &other == sig; });
        if (iter != msg.signals().end())
        {
            parent = &msg;
            break;
        }
    }
    return parent;
}
std::string_view Network::version()
{
    return _version;
}
std::pmr::vector<std::pmr::string>& Network::newSymbols()
{
    return _new_symbols;
}
BitTiming& Network::bitTiming()
{
    return _bit_timing;
}
std::pmr::vector<Node>& Network::nodes()
{
    return _nodes;
}
std::pmr::vector<ValueTable>& Network::valueTables()
{
    return _value_tables;
}
std::pmr::vector<Message>& Network::messages()
{
    return _messages;
}
std::pmr::vector<EnvironmentVariable>& Network::environmentVariables()
{
    return _environment_variables;
}
std::pmr::vector<AttributeDefinition>& Network::attributeDefinitions()
{
    return _attribute_definitions;
}
std::pmr::vector<Attribute>& Network::attributeDefaults()
{
    return _attribute_defaults;
}
std::pmr::vector<Attribute>& Network::attributeValues()
{
    return _attribute_values;
}
std::string_view Network::comment()
{
    return _comment;
}
void Network::Merge(Network&& other)
{
    auto& o = other;
    for (auto& ns : o.newSymbols())
    {
        newSymbols().push_back(std::move(ns));
    }
    for (auto& n : o.nodes())
    {
        nodes().push_back(std::move(n));
    }
    for (auto& vt : o.valueTables())
    {
        valueTables().push_back(std::move(vt));
    }
    for (auto& m : o.messages())
    {
        messages().push_back(std::move(m));
    }
    for (auto& ev : o.environmentVariables())
    {
        environmentVariables().push_back(std::move(ev));
    }
    for (auto& ad : o.attributeDefinitions())
    {
        attributeDefinitions().push_back(std::move(ad));
    }
    for (auto& ad : o.attributeDefaults())
    {
        attributeDefaults().push_back(std::move(ad));
    }
    for (auto& av : o.attributeValues())
    {
        attributeValues().push_back(std::move(av));
    }
}
bool Network::operator==(const Network& rhs) const
{
    bool equal = true;
    equal &= _version == rhs.Version();
    for (const auto& new_symbol : _new_symbols)
    {
        equal &= std::ranges::find(_new_symbols.begin(), _new_symbols.end(), new_symbol) != _new_symbols.end();
    }
    equal &= _bit_timing == rhs.GetBitTiming();
    for (const auto& node : rhs.Nodes())
    {
        equal &= std::ranges::find(_nodes.begin(), _nodes.end(), node) != _nodes.end();
    }
    for (const auto& value_table : rhs.ValueTables())
    {
        equal &= std::ranges::find(_value_tables.begin(), _value_tables.end(), value_table) != _value_tables.end();
    }
    for (const auto& message : rhs.Messages())
    {
        equal &= std::ranges::find(_messages.begin(), _messages.end(), message) != _messages.end();
    }
    for (const auto& env_var : rhs.EnvironmentVariables())
    {
        equal &= std::ranges::find(_environment_variables.begin(), _environment_variables.end(), env_var) != _environment_variables.end();
    }
    for (const auto& attr_def : rhs.AttributeDefinitions())
    {
        equal &= std::ranges::find(_attribute_definitions.begin(), _attribute_definitions.end(), attr_def) != _attribute_definitions.end();
    }
    for (const auto& attr : rhs.AttributeDefaults())
    {
        equal &= std::ranges::find(_attribute_defaults.begin(), _attribute_defaults.end(), attr) != _attribute_defaults.end();
    }
    for (const auto& attr : rhs.AttributeValues())
    {
        equal &= std::ranges::find(_attribute_values.begin(), _attribute_values.end(), attr) != _attribute_values.end();
    }
    equal &= _comment == rhs.Comment();
    return equal;
}
bool Network::operator!=(const Network& rhs) const
{
    return !(*this == rhs);
}
void Network::AddMessage(Message&& msg) {
    _messages.push_back(std::move(msg));
}
