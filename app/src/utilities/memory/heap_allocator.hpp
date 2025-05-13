#pragma once

#include <cstddef>

#ifdef CONFIG_SHARED_MULTI_HEAP
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <soc/soc_memory_layout.h>
#include <zephyr/multi_heap/shared_multi_heap.h>
#endif

namespace eerie_leap::utilities::memory {

template <typename T>
class HeapAllocator {
private:
    static constexpr size_t kAlignment = alignof(T) > 32 ? alignof(T) : 32;

public:
    using value_type = T;

    HeapAllocator() = default;

    template <typename U>
    constexpr HeapAllocator(const HeapAllocator<U>& obj) noexcept {}

    T* allocate(size_t n) {
    #ifdef CONFIG_SHARED_MULTI_HEAP
        return static_cast<T*>(shared_multi_heap_aligned_alloc(SMH_REG_ATTR_EXTERNAL, kAlignment, n * sizeof(T)));
    #else
        return static_cast<T*>(::operator new(n * sizeof(T)));
    #endif
    }

    void deallocate(T* p, size_t) noexcept {
    #ifdef CONFIG_SHARED_MULTI_HEAP
        shared_multi_heap_free(p);
    #else
        ::operator delete(p);
    #endif
    }
};

template <class T, class U>
bool operator==(const HeapAllocator<T>& lhs, const HeapAllocator<U>& rhs) { return true; }

template <class T, class U>
bool operator!=(const HeapAllocator<T>& lhs, const HeapAllocator<U>& rhs) { return false; }

} // namespace eerie_leap::utilities::memory
