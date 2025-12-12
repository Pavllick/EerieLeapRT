#pragma once

#include <memory_resource>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>
#include <functional>

#include "Iterator.h"
#include "Node.h"
#include "Attribute.h"
#include "ValueEncodingDescription.h"

namespace dbcppp
{
    class IEnvironmentVariable
    {
    public:
        enum class EVarType
        {
            Integer, Float, String, Data
        };
        enum class EAccessType
        {
            Unrestricted    = 0x0000,
            Read            = 0x0001,
            Write           = 0x0002,
            ReadWrite       = 0x0003,
            Unrestricted_   = 0x8000,
            Read_           = 0x8001,
            Write_          = 0x8002,
            ReadWrite_      = 0x8003
        };

        static pmr_unique_ptr<IEnvironmentVariable> Create(
              std::pmr::memory_resource* mr
            , std::pmr::string&& name
            , EVarType var_type
            , double minimum
            , double maximum
            , std::pmr::string&& unit
            , double initial_value
            , uint64_t ev_id
            , EAccessType access_type
            , std::pmr::vector<std::pmr::string>&& access_nodes
            , std::pmr::vector<pmr_unique_ptr<IValueEncodingDescription>>&& value_encoding_descriptions
            , uint64_t data_size
            , std::pmr::vector<pmr_unique_ptr<IAttribute>>&& attribute_values
            , std::pmr::string&& comment);

        virtual pmr_unique_ptr<IEnvironmentVariable> Clone() const = 0;

        virtual ~IEnvironmentVariable() = default;
        virtual const std::string_view Name() const = 0;
        virtual EVarType VarType() const = 0;
        virtual double Minimum() const = 0;
        virtual double Maximum() const = 0;
        virtual const std::string_view Unit() const = 0;
        virtual double InitialValue() const = 0;
        virtual uint64_t EvId() const = 0;
        virtual EAccessType AccessType() const = 0;
        virtual const std::pmr::string& AccessNodes_Get(std::size_t i) const = 0;
        virtual uint64_t AccessNodes_Size() const = 0;
        virtual const IValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const = 0;
        virtual uint64_t ValueEncodingDescriptions_Size() const = 0;
        virtual uint64_t DataSize() const = 0;
        virtual const IAttribute& AttributeValues_Get(std::size_t i) const = 0;
        virtual uint64_t AttributeValues_Size() const = 0;
        virtual const std::string_view Comment() const = 0;

        DBCPPP_MAKE_ITERABLE(IEnvironmentVariable, AccessNodes, std::pmr::string);
        DBCPPP_MAKE_ITERABLE(IEnvironmentVariable, ValueEncodingDescriptions, IValueEncodingDescription);
        DBCPPP_MAKE_ITERABLE(IEnvironmentVariable, AttributeValues, IAttribute);

        virtual bool operator==(const IEnvironmentVariable& rhs) const = 0;
        virtual bool operator!=(const IEnvironmentVariable& rhs) const = 0;
    };
}
