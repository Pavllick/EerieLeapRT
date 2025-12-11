#pragma once

#include <memory_resource>
#include <string>
#include <vector>
#include <istream>

#include "subsys/dbc/dbcppp/include/Iterator.h"
#include "subsys/dbc/dbcppp/include/BitTiming.h"
#include "subsys/dbc/dbcppp/include/ValueTable.h"
#include "subsys/dbc/dbcppp/include/Node.h"
#include "subsys/dbc/dbcppp/include/Message.h"
#include "subsys/dbc/dbcppp/include/EnvironmentVariable.h"
#include "subsys/dbc/dbcppp/include/AttributeDefinition.h"
#include "subsys/dbc/dbcppp/include/Attribute.h"

namespace dbcppp
{
    class INetwork
    {
    public:
        static pmr_unique_ptr<INetwork> Create(
              std::pmr::memory_resource* mr
            , std::pmr::string&& version
            , std::pmr::vector<std::pmr::string>&& new_symbols
            , std::unique_ptr<IBitTiming>&& bit_timing
            , std::pmr::vector<std::unique_ptr<INode>>&& nodes
            , std::pmr::vector<std::unique_ptr<IValueTable>>&& value_tables
            , std::pmr::vector<pmr_unique_ptr<IMessage>>&& messages
            , std::pmr::vector<std::unique_ptr<IEnvironmentVariable>>&& environment_variables
            , std::pmr::vector<std::unique_ptr<IAttributeDefinition>>&& attribute_definitions
            , std::pmr::vector<std::unique_ptr<IAttribute>>&& attribute_defaults
            , std::pmr::vector<std::unique_ptr<IAttribute>>&& attribute_values
            , std::pmr::string&& comment);
        static pmr_unique_ptr<INetwork> LoadDBCFromIs(std::pmr::memory_resource* mr, std::istream &is);

        virtual ~INetwork() = default;
        virtual const std::string_view Version() const = 0;
        virtual const std::pmr::string& NewSymbols_Get(std::size_t i) const = 0;
        virtual uint64_t NewSymbols_Size() const = 0;
        virtual const IBitTiming& BitTiming() const = 0;
        virtual const INode& Nodes_Get(std::size_t i) const = 0;
        virtual uint64_t Nodes_Size() const = 0;
        virtual const IValueTable& ValueTables_Get(std::size_t i) const = 0;
        virtual uint64_t ValueTables_Size() const = 0;
        virtual const IMessage& Messages_Get(std::size_t i) const = 0;
        virtual uint64_t Messages_Size() const = 0;
        virtual const IEnvironmentVariable& EnvironmentVariables_Get(std::size_t i) const = 0;
        virtual uint64_t EnvironmentVariables_Size() const = 0;
        virtual const IAttributeDefinition& AttributeDefinitions_Get(std::size_t i) const = 0;
        virtual uint64_t AttributeDefinitions_Size() const = 0;
        virtual const IAttribute& AttributeDefaults_Get(std::size_t i) const = 0;
        virtual uint64_t AttributeDefaults_Size() const = 0;
        virtual const IAttribute& AttributeValues_Get(std::size_t i) const = 0;
        virtual uint64_t AttributeValues_Size() const = 0;
        virtual const std::string_view Comment() const = 0;

        DBCPPP_MAKE_ITERABLE(INetwork, NewSymbols, std::pmr::string);
        DBCPPP_MAKE_ITERABLE(INetwork, Nodes, INode);
        DBCPPP_MAKE_ITERABLE(INetwork, ValueTables, IValueTable);
        DBCPPP_MAKE_ITERABLE(INetwork, Messages, IMessage);
        DBCPPP_MAKE_ITERABLE(INetwork, EnvironmentVariables, IEnvironmentVariable);
        DBCPPP_MAKE_ITERABLE(INetwork, AttributeDefinitions, IAttributeDefinition);
        DBCPPP_MAKE_ITERABLE(INetwork, AttributeDefaults, IAttribute);
        DBCPPP_MAKE_ITERABLE(INetwork, AttributeValues, IAttribute);

        virtual const IMessage* ParentMessage(const ISignal* sig) const = 0;

        virtual bool operator==(const INetwork& rhs) const = 0;
        virtual bool operator!=(const INetwork& rhs) const = 0;

        void Merge(std::unique_ptr<INetwork>&& other);
    };
}
