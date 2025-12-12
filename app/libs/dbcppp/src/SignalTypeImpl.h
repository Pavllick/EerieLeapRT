#pragma once

#include <cstdint>
#include "dbcppp/SignalType.h"

namespace dbcppp
{
    class SignalTypeImpl final
        : public ISignalType
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        SignalTypeImpl(
              std::allocator_arg_t
            , allocator_type alloc
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

        SignalTypeImpl& operator=(const SignalTypeImpl&) noexcept = default;
        SignalTypeImpl& operator=(SignalTypeImpl&&) noexcept = default;
        SignalTypeImpl(SignalTypeImpl&&) noexcept = default;
        ~SignalTypeImpl() = default;

        SignalTypeImpl(SignalTypeImpl&& other, allocator_type alloc)
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

        SignalTypeImpl(const SignalTypeImpl& other, allocator_type alloc = {})
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

        pmr_unique_ptr<ISignalType> Clone() const override;

        const std::string_view Name() const override;
        uint64_t SignalSize() const override;
        ISignal::EByteOrder ByteOrder() const override;
        ISignal::EValueType ValueType() const override;
        double Factor() const override;
        double Offset() const override;
        double Minimum() const override;
        double Maximum() const override;
        const std::string_view Unit() const override;
        double DefaultValue() const override;
        const std::string_view ValueTable() const override;

        bool operator==(const ISignalType& rhs) const override;
        bool operator!=(const ISignalType& rhs) const override;

    private:
        std::pmr::string _name;
        uint64_t _signal_size;
        ISignal::EByteOrder _byte_order;
        ISignal::EValueType _value_type;
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
