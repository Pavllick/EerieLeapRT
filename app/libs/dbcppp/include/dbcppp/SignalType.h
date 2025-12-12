#pragma once

#include <memory_resource>
#include <cstddef>
#include <string>
#include <memory>

#include "Signal.h"

namespace dbcppp
{
    class ISignalType
    {
    public:
        static pmr_unique_ptr<ISignalType> Create(
              std::pmr::memory_resource* mr
            , std::pmr::string&& name
            , uint64_t signal_size
            , ISignal::EByteOrder byte_order
            , ISignal::EValueType value_type
            , double factor
            , double offset
            , double minimum
            , double maximum
            , std::pmr::string&& unit
            , double default_value
            , std::pmr::string&& value_table);

        virtual pmr_unique_ptr<ISignalType> Clone() const = 0;

        virtual ~ISignalType() = default;
        virtual const std::string_view Name() const = 0;
        virtual uint64_t SignalSize() const = 0;
        virtual ISignal::EByteOrder ByteOrder() const = 0;
        virtual ISignal::EValueType ValueType() const = 0;
        virtual double Factor() const = 0;
        virtual double Offset() const = 0;
        virtual double Minimum() const = 0;
        virtual double Maximum() const = 0;
        virtual const std::string_view Unit() const = 0;
        virtual double DefaultValue() const = 0;
        virtual const std::string_view ValueTable() const = 0;

        virtual bool operator==(const ISignalType& rhs) const = 0;
        virtual bool operator!=(const ISignalType& rhs) const = 0;
    };
}
