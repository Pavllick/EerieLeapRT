#pragma once

#include <memory_resource>
#include <string>
#include <string_view>
#include <vector>
#include <cstddef>

#include "Iterator.h"
#include "Attribute.h"
#include "ValueEncodingDescription.h"

namespace dbcppp
{
    class EnvironmentVariable final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

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

        EnvironmentVariable(
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
            , std::pmr::vector<ValueEncodingDescription>&& value_encoding_descriptions
            , uint64_t data_size
            , std::pmr::vector<Attribute>&& attribute_values
            , std::pmr::string&& comment);

        EnvironmentVariable& operator=(const EnvironmentVariable&) noexcept = default;
        EnvironmentVariable& operator=(EnvironmentVariable&&) noexcept = default;
        EnvironmentVariable(EnvironmentVariable&&) noexcept = default;
        ~EnvironmentVariable() = default;

        EnvironmentVariable(EnvironmentVariable&& other, allocator_type alloc)
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

        EnvironmentVariable(const EnvironmentVariable& other, allocator_type alloc = {})
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

        EnvironmentVariable Clone() const;

        const std::string_view Name() const;
        EVarType VarType() const;
        double Minimum() const;
        double Maximum() const;
        const std::string_view Unit() const;
        double InitialValue() const;
        uint64_t EvId() const;
        EAccessType AccessType() const;
        const std::pmr::string& AccessNodes_Get(std::size_t i) const;
        std::size_t AccessNodes_Size() const;
        const ValueEncodingDescription& ValueEncodingDescriptions_Get(std::size_t i) const;
        std::size_t ValueEncodingDescriptions_Size() const;
        uint64_t DataSize() const;
        const Attribute& AttributeValues_Get(std::size_t i) const;
        std::size_t AttributeValues_Size() const;
        const std::string_view Comment() const;

        bool operator==(const EnvironmentVariable& rhs) const;
        bool operator!=(const EnvironmentVariable& rhs) const;

        DBCPPP_MAKE_ITERABLE(EnvironmentVariable, AccessNodes, std::pmr::string);
        DBCPPP_MAKE_ITERABLE(EnvironmentVariable, ValueEncodingDescriptions, ValueEncodingDescription);
        DBCPPP_MAKE_ITERABLE(EnvironmentVariable, AttributeValues, Attribute);

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
        std::pmr::vector<ValueEncodingDescription> _value_encoding_descriptions;
        uint64_t _data_size;
        std::pmr::vector<Attribute> _attribute_values;
        std::pmr::string _comment;

        allocator_type _allocator;
    };
}
