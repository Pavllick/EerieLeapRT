#pragma once

#include <memory_resource>
#include <cstddef>
#include <string>
#include <vector>
#include <functional>
#include <variant>
#include <memory>

#include "Iterator.h"

#include <eerie_memory.hpp>

using namespace eerie_memory;

namespace dbcppp
{
    class IAttributeDefinition
    {
    public:
        enum class EObjectType : std::uint8_t
        {
            Network,
            Node,
            Message,
            Signal,
            EnvironmentVariable,
        };
        struct ValueTypeInt
        {
            int64_t minimum;
            int64_t maximum;
        };
        struct ValueTypeHex
        {
            int64_t minimum;
            int64_t maximum;
        };
        struct ValueTypeFloat
        {
            double minimum;
            double maximum;
        };
        struct ValueTypeString
        {
        };
        struct ValueTypeEnum
        {
            std::vector<std::string> values;
        };
        using value_type_t = std::variant<ValueTypeInt, ValueTypeHex, ValueTypeFloat, ValueTypeString, ValueTypeEnum>;

        static pmr_unique_ptr<IAttributeDefinition> Create(
              std::pmr::memory_resource* mr
            , std::pmr::string&& name
            , EObjectType object_type
            , value_type_t&& value_type);

        virtual pmr_unique_ptr<IAttributeDefinition> Clone() const = 0;

        virtual ~IAttributeDefinition() = default;
        virtual EObjectType ObjectType() const = 0;
        virtual const std::string_view Name() const = 0;
        virtual const value_type_t& ValueType() const = 0;

        virtual bool operator==(const IAttributeDefinition& rhs) const = 0;
        virtual bool operator!=(const IAttributeDefinition& rhs) const = 0;
    };
}
