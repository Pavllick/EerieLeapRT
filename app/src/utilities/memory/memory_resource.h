#pragma once

#include <cstddef>
#include <memory_resource>

#include "heap_allocator.h"

namespace eerie_leap::utilities::memory {

class ExtMemoryResource : public std::pmr::memory_resource {
private:
    HeapAllocator<std::byte> allocator_;

public:
    void* do_allocate(size_t bytes, size_t align) override {
        return allocator_.allocate(bytes, align);
    }

    void do_deallocate(void* ptr, size_t bytes, size_t align) override {
        allocator_.deallocate(ptr, bytes, align);
    }

    bool do_is_equal(std::pmr::memory_resource const& other) const noexcept override {
        return &other == this;
    }
};

} // namespace eerie_leap::utilities::memory
