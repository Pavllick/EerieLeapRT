#pragma once

#include <cstdint>

namespace dbcppp
{
    class BitTiming final
    {
    public:
        BitTiming();
        BitTiming(
              uint64_t baudrate
            , uint64_t BTR1
            , uint64_t BTR2);

        BitTiming& operator=(const BitTiming&) noexcept = default;
        BitTiming& operator=(BitTiming&&) noexcept = default;
        BitTiming(const BitTiming&) noexcept = default;
        BitTiming(BitTiming&&) noexcept = default;
        ~BitTiming() = default;

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
    };
}
