#pragma once

#include <memory_resource>
#include <memory>
#include <cstdint>

#include <eerie_memory.hpp>

using namespace eerie_memory;

namespace dbcppp
{
    class IBitTiming
    {
    public:
        static pmr_unique_ptr<IBitTiming> Create(
              std::pmr::memory_resource* mr
            , uint64_t baudrate
            , uint64_t BTR1
            , uint64_t BTR2);

        virtual pmr_unique_ptr<IBitTiming> Clone() const = 0;

        virtual ~IBitTiming() = default;
        virtual uint64_t Baudrate() const = 0;
        virtual uint64_t BTR1() const = 0;
        virtual uint64_t BTR2() const = 0;

        virtual bool operator==(const IBitTiming& rhs) const = 0;
        virtual bool operator!=(const IBitTiming& rhs) const = 0;
    };
}
