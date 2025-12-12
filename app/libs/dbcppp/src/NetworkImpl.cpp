#include <algorithm>
#include <memory>
#include "dbcppp/Network.h"
#include "dbcppp/Signal.h"
#include "dbcppp/NetworkImpl.h"

using namespace dbcppp;

pmr_unique_ptr<INetwork> INetwork::Create(
      std::pmr::memory_resource* mr
    , std::pmr::string&& version
    , std::pmr::vector<std::pmr::string>&& new_symbols
    , pmr_unique_ptr<IBitTiming>&& bit_timing
    , std::pmr::vector<pmr_unique_ptr<INode>>&& nodes
    , std::pmr::vector<pmr_unique_ptr<IValueTable>>&& value_tables
    , std::pmr::vector<pmr_unique_ptr<IMessage>>&& messages
    , std::pmr::vector<pmr_unique_ptr<IEnvironmentVariable>>&& environment_variables
    , std::pmr::vector<pmr_unique_ptr<IAttributeDefinition>>&& attribute_definitions
    , std::pmr::vector<pmr_unique_ptr<IAttribute>>&& attribute_defaults
    , std::pmr::vector<pmr_unique_ptr<IAttribute>>&& attribute_values
    , std::pmr::string&& comment)
{
    BitTimingImpl bt = std::move(static_cast<BitTimingImpl&>(*bit_timing));
    bit_timing.reset(nullptr);
    std::pmr::vector<NodeImpl> ns(mr);
    std::pmr::vector<ValueTableImpl> vts(mr);
    std::pmr::vector<MessageImpl> ms;
    std::pmr::vector<EnvironmentVariableImpl> evs(mr);
    std::pmr::vector<AttributeDefinitionImpl> ads(mr);
    std::pmr::vector<AttributeImpl> avds(mr);
    std::pmr::vector<AttributeImpl> avs(mr);
    for (auto& n : nodes)
    {
        ns.push_back(std::move(static_cast<NodeImpl&>(*n)));
        n.reset(nullptr);
    }
    for (auto& vt : value_tables)
    {
        vts.push_back(std::move(static_cast<ValueTableImpl&>(*vt)));
        vt.reset(nullptr);
    }
    for (auto& m : messages)
    {
        ms.push_back(std::move(static_cast<MessageImpl&>(*m)));
        m.reset(nullptr);
    }
    for (auto& ev : environment_variables)
    {
        evs.push_back(std::move(static_cast<EnvironmentVariableImpl&>(*ev)));
        ev.reset(nullptr);
    }
    for (auto& ad : attribute_definitions)
    {
        ads.push_back(std::move(static_cast<AttributeDefinitionImpl&>(*ad)));
        ad.reset(nullptr);
    }
    for (auto& ad : attribute_defaults)
    {
        avds.push_back(std::move(static_cast<AttributeImpl&>(*ad)));
        ad.reset(nullptr);
    }
    for (auto& av : attribute_values)
    {
        avs.push_back(std::move(static_cast<AttributeImpl&>(*av)));
        av.reset(nullptr);
    }
    return make_unique_pmr<NetworkImpl>(
          mr
        , std::move(version)
        , std::move(new_symbols)
        , std::move(bt)
        , std::move(ns)
        , std::move(vts)
        , std::move(ms)
        , std::move(evs)
        , std::move(ads)
        , std::move(avds)
        , std::move(avs)
        , std::move(comment));
}

NetworkImpl::NetworkImpl(
      std::allocator_arg_t alloc_arg
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
    , std::pmr::string&& comment)

    : _version(std::move(version), alloc)
    , _new_symbols(std::move(new_symbols), alloc)
    , _bit_timing(std::move(bit_timing))
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
pmr_unique_ptr<INetwork> NetworkImpl::Clone() const
{
    return make_unique_pmr< NetworkImpl>(_allocator, *this);
}
const std::string_view NetworkImpl::Version() const
{
    return _version;
}
const std::pmr::string& NetworkImpl::NewSymbols_Get(std::size_t i) const
{
    return _new_symbols[i];
}
std::size_t NetworkImpl::NewSymbols_Size() const
{
    return _new_symbols.size();
}
const IBitTiming& NetworkImpl::BitTiming() const
{
    return _bit_timing;
}
const INode& NetworkImpl::Nodes_Get(std::size_t i) const
{
    return _nodes[i];
}
std::size_t NetworkImpl::Nodes_Size() const
{
    return _nodes.size();
}
const IValueTable& NetworkImpl::ValueTables_Get(std::size_t i) const
{
    return _value_tables[i];
}
std::size_t NetworkImpl::ValueTables_Size() const
{
    return _value_tables.size();
}
const IMessage& NetworkImpl::Messages_Get(std::size_t i) const
{
    return _messages[i];
}
std::size_t NetworkImpl::Messages_Size() const
{
    return _messages.size();
}
const IEnvironmentVariable& NetworkImpl::EnvironmentVariables_Get(std::size_t i) const
{
    return _environment_variables[i];
}
std::size_t NetworkImpl::EnvironmentVariables_Size() const
{
    return _environment_variables.size();
}
const IAttributeDefinition& NetworkImpl::AttributeDefinitions_Get(std::size_t i) const
{
    return _attribute_definitions[i];
}
std::size_t NetworkImpl::AttributeDefinitions_Size() const
{
    return _attribute_definitions.size();
}
const IAttribute& NetworkImpl::AttributeDefaults_Get(std::size_t i) const
{
    return _attribute_defaults[i];
}
std::size_t NetworkImpl::AttributeDefaults_Size() const
{
    return _attribute_defaults.size();
}
const IAttribute& NetworkImpl::AttributeValues_Get(std::size_t i) const
{
    return _attribute_values[i];
}
std::size_t NetworkImpl::AttributeValues_Size() const
{
    return _attribute_values.size();
}
const std::string_view NetworkImpl::Comment() const
{
    return _comment;
}
const IMessage* NetworkImpl::ParentMessage(const ISignal* sig) const
{
    const IMessage* parent = nullptr;
    for (const auto& msg : _messages)
    {
        auto iter = std::find_if(msg.signals().begin(), msg.signals().end(),
            [&](const SignalImpl& other) { return &other == sig; });
        if (iter != msg.signals().end())
        {
            parent = &msg;
            break;
        }
    }
    return parent;
}
std::string_view NetworkImpl::version()
{
    return _version;
}
std::pmr::vector<std::pmr::string>& NetworkImpl::newSymbols()
{
    return _new_symbols;
}
BitTimingImpl& NetworkImpl::bitTiming()
{
    return _bit_timing;
}
std::pmr::vector<NodeImpl>& NetworkImpl::nodes()
{
    return _nodes;
}
std::pmr::vector<ValueTableImpl>& NetworkImpl::valueTables()
{
    return _value_tables;
}
std::pmr::vector<MessageImpl>& NetworkImpl::messages()
{
    return _messages;
}
std::pmr::vector<EnvironmentVariableImpl>& NetworkImpl::environmentVariables()
{
    return _environment_variables;
}
std::pmr::vector<AttributeDefinitionImpl>& NetworkImpl::attributeDefinitions()
{
    return _attribute_definitions;
}
std::pmr::vector<AttributeImpl>& NetworkImpl::attributeDefaults()
{
    return _attribute_defaults;
}
std::pmr::vector<AttributeImpl>& NetworkImpl::attributeValues()
{
    return _attribute_values;
}
std::string_view NetworkImpl::comment()
{
    return _comment;
}
void INetwork::Merge(pmr_unique_ptr<INetwork>&& other)
{
    auto& self = static_cast<NetworkImpl&>(*this);
    auto& o = static_cast<NetworkImpl&>(*other);
    for (auto& ns : o.newSymbols())
    {
        self.newSymbols().push_back(std::move(ns));
    }
    for (auto& n : o.nodes())
    {
        self.nodes().push_back(std::move(n));
    }
    for (auto& vt : o.valueTables())
    {
        self.valueTables().push_back(std::move(vt));
    }
    for (auto& m : o.messages())
    {
        self.messages().push_back(std::move(m));
    }
    for (auto& ev : o.environmentVariables())
    {
        self.environmentVariables().push_back(std::move(ev));
    }
    for (auto& ad : o.attributeDefinitions())
    {
        self.attributeDefinitions().push_back(std::move(ad));
    }
    for (auto& ad : o.attributeDefaults())
    {
        self.attributeDefaults().push_back(std::move(ad));
    }
    for (auto& av : o.attributeValues())
    {
        self.attributeValues().push_back(std::move(av));
    }
    other.reset(nullptr);
}
bool NetworkImpl::operator==(const INetwork& rhs) const
{
    bool equal = true;
    equal &= _version == rhs.Version();
    for (const auto& new_symbol : _new_symbols)
    {
        equal &= std::ranges::find(_new_symbols.begin(), _new_symbols.end(), new_symbol) != _new_symbols.end();
    }
    equal &= _bit_timing == rhs.BitTiming();
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
bool NetworkImpl::operator!=(const INetwork& rhs) const
{
    return !(*this == rhs);
}
