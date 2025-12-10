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

template<typename T, typename... Args>
std::shared_ptr<T> make_shared_pmr(std::pmr::memory_resource* mr, Args&&... args) {
    return std::allocate_shared<T>(std::pmr::polymorphic_allocator<T>(mr), std::forward<Args>(args)...);
}

template<class T>
class pmr_deleter {
private:
    std::pmr::memory_resource* mr_;

public:
    pmr_deleter(std::pmr::memory_resource* mr = std::pmr::get_default_resource())
        : mr_(mr) {}

    pmr_deleter(pmr_deleter const&) noexcept = default;
    pmr_deleter& operator=(pmr_deleter const&) noexcept = default;
    pmr_deleter(pmr_deleter&&) noexcept = default;
    pmr_deleter& operator=(pmr_deleter&&) noexcept = default;

    void operator()(T* p) const noexcept {
        if(!p)
            return;

        std::destroy_at(p);
        mr_->deallocate(p, sizeof(T), alignof(T));
    }
};

template<typename T>
using pmr_unique_ptr = std::unique_ptr<T, pmr_deleter<T>>;

template<typename T, typename... Args>
pmr_unique_ptr<T> make_unique_pmr(std::pmr::memory_resource* mr, Args&&... args) {
    std::pmr::polymorphic_allocator<T> prm_allocator(mr);
    T* ptr = prm_allocator.allocate(1);

    try {
        prm_allocator.construct(ptr, std::forward<Args>(args)...);
        return pmr_unique_ptr<T>(ptr, pmr_deleter<T>(mr));
    } catch (...) {
        prm_allocator.deallocate(ptr, 1);
        throw;
    }
}

} // namespace eerie_leap::utilities::memory
