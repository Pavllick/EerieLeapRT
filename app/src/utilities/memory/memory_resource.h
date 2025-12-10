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

template<typename T, typename... Args>
std::shared_ptr<T> make_shared_pmr(const std::pmr::polymorphic_allocator<std::byte>& alloc, Args&&... args) {
    return std::allocate_shared<T>(alloc, std::forward<Args>(args)...);
}

template<class T>
class pmr_deleter {
private:
    std::pmr::memory_resource* mr_;
    size_t size_;
    size_t alignment_;

public:
    pmr_deleter(
        std::pmr::memory_resource* mr = std::pmr::get_default_resource(),
        size_t size = sizeof(T),
        size_t alignment = alignof(T))
        : mr_(mr), size_(size), alignment_(alignment) {}

    pmr_deleter(pmr_deleter const&) noexcept = default;
    pmr_deleter& operator=(pmr_deleter const&) noexcept = default;
    pmr_deleter(pmr_deleter&&) noexcept = default;
    pmr_deleter& operator=(pmr_deleter&&) noexcept = default;

    template<class U>
        requires std::convertible_to<U*, T*>
    pmr_deleter(const pmr_deleter<U>& other) noexcept
        : mr_(other.mr_), size_(other.size_), alignment_(other.alignment_) {}

    template<class U>
        requires std::convertible_to<U*, T*>
    pmr_deleter(pmr_deleter<U>&& other) noexcept
        : mr_(other.mr_), size_(other.size_), alignment_(other.alignment_) {}

    void operator()(T* p) const noexcept {
        if(!p)
            return;

        std::destroy_at(p);
        mr_->deallocate(p, size_, alignment_);
    }

    // Make all instantiations friends of each other
    template<class U>
    friend class pmr_deleter;
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

template<typename T, typename... Args>
pmr_unique_ptr<T> make_unique_pmr(std::pmr::polymorphic_allocator<std::byte> alloc, Args&&... args) {
    return make_unique_pmr<T>(alloc.resource(), std::forward<Args>(args)...);
}

} // namespace eerie_leap::utilities::memory
