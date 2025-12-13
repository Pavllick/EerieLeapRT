#pragma once

#include <memory_resource>
#include <cstdint>
#include <string>

#include "Signal.h"

namespace dbcppp
{
    class SignalType final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        SignalType(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , uint64_t signal_size
            , Signal::EByteOrder byte_order
            , Signal::EValueType value_type
            , double factor
            , double offset
            , double minimum
            , double maximum
            , std::pmr::string&& unit
            , double default_value
            , std::pmr::string&& value_table);

        SignalType& operator=(const SignalType&) noexcept = default;
        SignalType& operator=(SignalType&&) noexcept = default;
        SignalType(SignalType&&) noexcept = default;
        ~SignalType() = default;

        SignalType(SignalType&& other, allocator_type alloc)
            : _name(std::move(other._name), alloc)
            , _signal_size(other._signal_size)
            , _byte_order(other._byte_order)
            , _value_type(other._value_type)
            , _factor(other._factor)
            , _offset(other._offset)
            , _minimum(other._minimum)
            , _maximum(other._maximum)
            , _unit(std::move(other._unit), alloc)
            , _default_value(other._default_value)
            , _value_table(std::move(other._value_table), alloc)
            , _allocator(alloc) {}

        SignalType(const SignalType& other, allocator_type alloc = {})
            : _name(other._name, alloc)
            , _signal_size(other._signal_size)
            , _byte_order(other._byte_order)
            , _value_type(other._value_type)
            , _factor(other._factor)
            , _offset(other._offset)
            , _minimum(other._minimum)
            , _maximum(other._maximum)
            , _unit(other._unit, alloc)
            , _default_value(other._default_value)
            , _value_table(other._value_table, alloc) {}

        SignalType Clone() const;

        const std::string_view Name() const;
        uint64_t SignalSize() const;
        Signal::EByteOrder ByteOrder() const;
        Signal::EValueType ValueType() const;
        double Factor() const;
        double Offset() const;
        double Minimum() const;
        double Maximum() const;
        const std::string_view Unit() const;
        double DefaultValue() const;
        const std::string_view ValueTable() const;

        bool operator==(const SignalType& rhs) const;
        bool operator!=(const SignalType& rhs) const;

    private:
        std::pmr::string _name;
        uint64_t _signal_size;
        Signal::EByteOrder _byte_order;
        Signal::EValueType _value_type;
        double _factor;
        double _offset;
        double _minimum;
        double _maximum;
        std::pmr::string _unit;
        double _default_value;
        std::pmr::string _value_table;

        allocator_type _allocator;
    };
}
