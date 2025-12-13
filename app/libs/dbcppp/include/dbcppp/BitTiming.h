#pragma once

#include <memory_resource>
#include <cstdint>

namespace dbcppp
{
    class BitTiming final
    {
    public:
        using allocator_type = std::pmr::polymorphic_allocator<>;

        BitTiming(
            std::allocator_arg_t
            , allocator_type alloc
        );
        BitTiming(
              std::allocator_arg_t
            , allocator_type alloc
            , uint64_t baudrate
            , uint64_t BTR1
            , uint64_t BTR2);

        BitTiming& operator=(const BitTiming&) noexcept = default;
        BitTiming& operator=(BitTiming&&) noexcept = default;
        BitTiming(BitTiming&&) noexcept = default;
        ~BitTiming() = default;

        BitTiming(BitTiming&& other, allocator_type alloc)
            : _baudrate(other._baudrate)
            , _BTR1(other._BTR1)
            , _BTR2(other._BTR2)
            , _allocator(alloc) {}

        BitTiming(const BitTiming& other, allocator_type alloc = {})
            : _baudrate(other._baudrate)
            , _BTR1(other._BTR1)
            , _BTR2(other._BTR2)
            , _allocator(alloc) {}

        static BitTiming Create(
              std::pmr::memory_resource* mr
            , uint64_t baudrate
            , uint64_t BTR1
            , uint64_t BTR2);

        BitTiming Clone() const;

        uint64_t Baudrate() const;
        uint64_t BTR1() const;
        uint64_t BTR2() const;

        bool operator==(const BitTiming& rhs) const;
        bool operator!=(const BitTiming& rhs) const;

    private:
        uint64_t _baudrate;
        uint64_t _BTR1;
        uint64_t _BTR2;

        allocator_type _allocator;
    };
}
