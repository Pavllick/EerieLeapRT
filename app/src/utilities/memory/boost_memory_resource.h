#pragma once

#include <cstddef>
#include <boost/container/pmr/memory_resource.hpp>

#include "heap_allocator.h"

namespace eerie_leap::utilities::memory {

namespace pmr = boost::container::pmr;

class BoostMemoryResource : public pmr::memory_resource {
private:
    HeapAllocator<std::byte> allocator_;

public:
    void* do_allocate(std::size_t bytes, std::size_t align) override {
        size_t extra = align > alignof(std::byte) ? align : 0;
        size_t total_bytes = bytes + extra;

        std::byte* ptr = allocator_.allocate(total_bytes);

        if(extra > 0) {
            uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
            uintptr_t aligned = (addr + align - 1) & ~(align - 1);

            return reinterpret_cast<void*>(aligned);
        }

        return ptr;
    }

    void do_deallocate(void* ptr, std::size_t bytes, std::size_t align) override {
        if(!ptr)
            return;

        size_t extra = align > alignof(std::byte) ? align : 0;
        size_t total_bytes = bytes + extra;

        allocator_.deallocate(static_cast<std::byte*>(ptr), total_bytes);
    }

    bool do_is_equal(pmr::memory_resource const& other) const noexcept override {
        return dynamic_cast<BoostMemoryResource const*>(&other) != nullptr;
    }
};

static BoostMemoryResource ext_boost_mem_resource;

} // namespace eerie_leap::utilities::memory
