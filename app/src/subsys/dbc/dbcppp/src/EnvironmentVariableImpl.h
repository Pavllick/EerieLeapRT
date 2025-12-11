#pragma once

#include "subsys/dbc/dbcppp/include/EnvironmentVariable.h"
#include "AttributeImpl.h"
#include "ValueEncodingDescriptionImpl.h"

namespace dbcppp
{
    class EnvironmentVariableImpl final
        : public IEnvironmentVariable
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        EnvironmentVariableImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , std::pmr::string&& name
            , EVarType var_type
            , double minimum
            , double maximum
            , std::pmr::string&& unit
            , double initial_value
            , uint64_t ev_id
            , EAccessType access_type
            , std::pmr::vector<std::pmr::string>&& access_nodes
            , std::pmr::vector<ValueEncodingDescriptionImpl>&& value_encoding_descriptions
            , uint64_t data_size
            , std::pmr::vector<AttributeImpl>&& attribute_values
            , std::pmr::string&& comment);

        EnvironmentVariableImpl& operator=(const EnvironmentVariableImpl&) noexcept = default;
        EnvironmentVariableImpl& operator=(EnvironmentVariableImpl&&) noexcept = default;
        EnvironmentVariableImpl(EnvironmentVariableImpl&&) noexcept = default;
        ~EnvironmentVariableImpl() = default;

        EnvironmentVariableImpl(EnvironmentVariableImpl&& other, allocator_type alloc)
            : _name(std::move(other._name), alloc)
            , _var_type(other._var_type)
            , _minimum(other._minimum)
            , _maximum(other._maximum)
            , _unit(std::move(other._unit), alloc)
            , _initial_value(other._initial_value)
            , _ev_id(other._ev_id)
            , _access_type(other._access_type)
            , _access_nodes(std::move(other._access_nodes), alloc)
            , _value_encoding_descriptions(std::move(other._value_encoding_descriptions), alloc)
            , _data_size(other._data_size)
            , _attribute_values(std::move(other._attribute_values), alloc)
            , _comment(std::move(other._comment), alloc)
            , _allocator(alloc) {}

        EnvironmentVariableImpl(const EnvironmentVariableImpl& other, allocator_type alloc = {})
            : _name(other._name, alloc)
            , _var_type(other._var_type)
            , _minimum(other._minimum)
            , _maximum(other._maximum)
            , _unit(other._unit, alloc)
            , _initial_value(other._initial_value)
            , _ev_id(other._ev_id)
            , _access_type(other._access_type)
            , _access_nodes(other._access_nodes, alloc)
            , _value_encoding_descriptions(other._value_encoding_descriptions, alloc)
            , _data_size(other._data_size)
            , _attribute_values(other._attribute_values, alloc)
            , _comment(other._comment, alloc)
            , _allocator(alloc) {}

        virtual std::unique_ptr<IEnvironmentVariable> Clone() const override;

        virtual const std::string_view Name() const override;
        virtual EVarType VarType() const override;
        virtual double Minimum() const override;
        virtual double Maximum() const override;
        virtual const std::string_view Unit() const override;
        virtual double InitialValue() const override;
        virtual uint64_t EvId() const override;
        virtual EAccessType AccessType() const override;
        virtual const std::pmr::string& AccessNodes_Get(std::size_t i) const override;
        virtual uint64_t AccessNodes_Size() const override;
        virtual const IValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const override;
        virtual uint64_t ValueEncodingDescriptions_Size() const override;
        virtual uint64_t DataSize() const override;
        virtual const IAttribute& AttributeValues_Get(std::size_t i) const override;
        virtual uint64_t AttributeValues_Size() const override;
        virtual const std::string_view Comment() const override;

        virtual bool operator==(const IEnvironmentVariable& rhs) const override;
        virtual bool operator!=(const IEnvironmentVariable& rhs) const override;

    private:
        std::pmr::string _name;
        EVarType _var_type;
        double _minimum;
        double _maximum;
        std::pmr::string _unit;
        double _initial_value;
        uint64_t _ev_id;
        EAccessType _access_type;
        std::pmr::vector<std::pmr::string> _access_nodes;
        std::pmr::vector<ValueEncodingDescriptionImpl> _value_encoding_descriptions;
        uint64_t _data_size;
        std::pmr::vector<AttributeImpl> _attribute_values;
        std::pmr::string _comment;

        allocator_type _allocator;
    };
}
