#pragma once

#include <memory_resource>
#include <vector>
#include <string>
#include <memory>
#include <optional>

#include "SignalType.h"
#include "ValueEncodingDescription.h"

namespace dbcppp
{
    class IValueTable
    {
    public:
        static pmr_unique_ptr<IValueTable> Create(
              std::pmr::memory_resource* mr
            , std::pmr::string&& name
            , std::optional<pmr_unique_ptr<ISignalType>>&& signal_type
            , std::pmr::vector<pmr_unique_ptr<IValueEncodingDescription>>&& value_encoding_descriptions);

        virtual pmr_unique_ptr<IValueTable> Clone() const = 0;

        virtual ~IValueTable() = default;
        virtual const std::string_view Name() const = 0;
        virtual std::optional<std::reference_wrapper<const ISignalType>> SignalType() const = 0;
        virtual const IValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const = 0;
        virtual uint64_t ValueEncodingDescriptions_Size() const = 0;

        DBCPPP_MAKE_ITERABLE(IValueTable, ValueEncodingDescriptions, IValueEncodingDescription);

        virtual bool operator==(const IValueTable& rhs) const = 0;
        virtual bool operator!=(const IValueTable& rhs) const = 0;
    };
}
