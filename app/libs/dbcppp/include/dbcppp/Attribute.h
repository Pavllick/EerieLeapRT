#pragma once

#include <memory_resource>
#include <string>
#include <cstddef>
#include <variant>

#include "AttributeDefinition.h"

#include <eerie_memory.hpp>

using namespace eerie_memory;

namespace dbcppp
{
    class IAttribute
    {
    public:
        using hex_value_t = int64_t;
        using value_t = std::variant<int64_t, double, std::string>;

        static pmr_unique_ptr<IAttribute> Create(
              std::pmr::memory_resource* mr
            , std::pmr::string&& name
            , IAttributeDefinition::EObjectType object_type
            , value_t value);

        virtual pmr_unique_ptr<IAttribute> Clone() const = 0;

        virtual ~IAttribute() = default;
        virtual const std::string_view Name() const = 0;
        virtual IAttributeDefinition::EObjectType ObjectType() const = 0;
        virtual const value_t& Value() const = 0;

        virtual bool operator==(const IAttribute& rhs) const = 0;
        virtual bool operator!=(const IAttribute& rhs) const = 0;
    };
}
