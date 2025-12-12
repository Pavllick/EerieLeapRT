#pragma once

#include "dbcppp/BitTiming.h"

namespace dbcppp
{
    class BitTimingImpl final
        : public IBitTiming
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        BitTimingImpl(
            std::allocator_arg_t
            , allocator_type alloc
        );
        BitTimingImpl(
              std::allocator_arg_t
            , allocator_type alloc
            , uint64_t baudrate
            , uint64_t BTR1
            , uint64_t BTR2);

        BitTimingImpl& operator=(const BitTimingImpl&) noexcept = default;
        BitTimingImpl& operator=(BitTimingImpl&&) noexcept = default;
        BitTimingImpl(BitTimingImpl&&) noexcept = default;
        ~BitTimingImpl() = default;

        BitTimingImpl(BitTimingImpl&& other, allocator_type alloc)
            : _baudrate(other._baudrate)
            , _BTR1(other._BTR1)
            , _BTR2(other._BTR2)
            , _allocator(alloc) {}

        BitTimingImpl(const BitTimingImpl& other, allocator_type alloc = {})
            : _baudrate(other._baudrate)
            , _BTR1(other._BTR1)
            , _BTR2(other._BTR2)
            , _allocator(alloc) {}

        pmr_unique_ptr<IBitTiming> Clone() const override;

        uint64_t Baudrate() const override;
        uint64_t BTR1() const override;
        uint64_t BTR2() const override;

        bool operator==(const IBitTiming& rhs) const override;
        bool operator!=(const IBitTiming& rhs) const override;

    private:
        uint64_t _baudrate;
        uint64_t _BTR1;
        uint64_t _BTR2;

        allocator_type _allocator;
    };
}
