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
    class INetwork
    {
    public:
        static pmr_unique_ptr<INetwork> Create(
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
            , std::pmr::string&& comment);
        static pmr_unique_ptr<INetwork> LoadDBCFromIs(std::pmr::memory_resource* mr, std::istream &is);

        virtual pmr_unique_ptr<INetwork> Clone() const = 0;

        virtual ~INetwork() = default;
        virtual const std::string_view Version() const = 0;
        virtual const std::pmr::string& NewSymbols_Get(std::size_t i) const = 0;
        virtual std::size_t NewSymbols_Size() const = 0;
        virtual const IBitTiming& BitTiming() const = 0;
        virtual const INode& Nodes_Get(std::size_t i) const = 0;
        virtual std::size_t Nodes_Size() const = 0;
        virtual const IValueTable& ValueTables_Get(std::size_t i) const = 0;
        virtual std::size_t ValueTables_Size() const = 0;
        virtual const IMessage& Messages_Get(std::size_t i) const = 0;
        virtual std::size_t Messages_Size() const = 0;
        virtual const IEnvironmentVariable& EnvironmentVariables_Get(std::size_t i) const = 0;
        virtual std::size_t EnvironmentVariables_Size() const = 0;
        virtual const IAttributeDefinition& AttributeDefinitions_Get(std::size_t i) const = 0;
        virtual std::size_t AttributeDefinitions_Size() const = 0;
        virtual const IAttribute& AttributeDefaults_Get(std::size_t i) const = 0;
        virtual std::size_t AttributeDefaults_Size() const = 0;
        virtual const IAttribute& AttributeValues_Get(std::size_t i) const = 0;
        virtual std::size_t AttributeValues_Size() const = 0;
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

        void Merge(pmr_unique_ptr<INetwork>&& other);
    };
}
