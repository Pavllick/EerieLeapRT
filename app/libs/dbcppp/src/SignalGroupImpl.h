
#pragma once

#include "dbcppp/SignalGroup.h"

namespace dbcppp
{
    class SignalGroupImpl
        : public ISignalGroup
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        SignalGroupImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , uint64_t message_id
            , std::pmr::string&& name
            , uint64_t repetitions
            , std::pmr::vector<std::pmr::string>&& signal_names);

        SignalGroupImpl& operator=(const SignalGroupImpl&) noexcept = default;
        SignalGroupImpl& operator=(SignalGroupImpl&&) noexcept = default;
        SignalGroupImpl(SignalGroupImpl&&) noexcept = default;
        ~SignalGroupImpl() = default;

        SignalGroupImpl(SignalGroupImpl&& other, allocator_type alloc)
            : _message_id(other._message_id)
            , _name(std::move(other._name))
            , _repetitions(other._repetitions)
            , _signal_names(std::move(other._signal_names))
            , _allocator(alloc) {}

        SignalGroupImpl(const SignalGroupImpl& other, allocator_type alloc = {})
            : _message_id(other._message_id)
            , _name(other._name, alloc)
            , _repetitions(other._repetitions)
            , _signal_names(other._signal_names, alloc)
            , _allocator(alloc) {}

        pmr_unique_ptr<ISignalGroup> Clone() const override;

        uint64_t MessageId() const override;
        const std::string_view Name() const override;
        uint64_t Repetitions() const override;
        const std::pmr::string& SignalNames_Get(std::size_t i) const override;
        uint64_t SignalNames_Size() const override;

        bool operator==(const ISignalGroup& rhs) const override;
        bool operator!=(const ISignalGroup& rhs) const override;

    private:
        uint64_t _message_id;
        std::pmr::string _name;
        uint64_t _repetitions;
        std::pmr::vector<std::pmr::string> _signal_names;

        allocator_type _allocator;
    };
}
