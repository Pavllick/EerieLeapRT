#pragma once

#include <memory_resource>
#include <vector>
#include <string>
#include <cstdint>

#include "Iterator.h"

namespace dbcppp
{
    class SignalGroup
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        SignalGroup(
              std::allocator_arg_t
            , allocator_type alloc
            , uint64_t message_id
            , std::pmr::string&& name
            , uint64_t repetitions
            , std::pmr::vector<std::pmr::string>&& signal_names);

        SignalGroup& operator=(const SignalGroup&) noexcept = default;
        SignalGroup& operator=(SignalGroup&&) noexcept = default;
        SignalGroup(SignalGroup&&) noexcept = default;
        ~SignalGroup() = default;

        SignalGroup(SignalGroup&& other, allocator_type alloc)
            : _message_id(other._message_id)
            , _name(std::move(other._name))
            , _repetitions(other._repetitions)
            , _signal_names(std::move(other._signal_names))
            , _allocator(alloc) {}

        SignalGroup(const SignalGroup& other, allocator_type alloc = {})
            : _message_id(other._message_id)
            , _name(other._name, alloc)
            , _repetitions(other._repetitions)
            , _signal_names(other._signal_names, alloc)
            , _allocator(alloc) {}

        SignalGroup Clone() const;

        uint64_t MessageId() const;
        const std::string_view Name() const;
        uint64_t Repetitions() const;
        const std::pmr::string& SignalNames_Get(std::size_t i) const;
        std::size_t SignalNames_Size() const;

        bool operator==(const SignalGroup& rhs) const;
        bool operator!=(const SignalGroup& rhs) const;

        DBCPPP_MAKE_ITERABLE(SignalGroup, SignalNames, std::pmr::string);

    private:
        uint64_t _message_id;
        std::pmr::string _name;
        uint64_t _repetitions;
        std::pmr::vector<std::pmr::string> _signal_names;

        allocator_type _allocator;
    };
}
