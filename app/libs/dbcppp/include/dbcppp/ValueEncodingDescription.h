
#pragma once

#include <memory_resource>
#include <string>

#include <eerie_memory.hpp>

using namespace eerie_memory;

namespace dbcppp
{
    class IValueEncodingDescription
    {
    public:
        static pmr_unique_ptr<IValueEncodingDescription> Create(
              std::pmr::memory_resource* mr
            , int64_t value
            , std::pmr::string&& description);
        virtual pmr_unique_ptr<IValueEncodingDescription> Clone() const = 0;
        virtual ~IValueEncodingDescription() = default;

        virtual int64_t Value() const = 0;
        virtual const std::string_view Description() const = 0;

        virtual bool operator==(const IValueEncodingDescription& rhs) const = 0;
        virtual bool operator!=(const IValueEncodingDescription& rhs) const = 0;
    };
}
