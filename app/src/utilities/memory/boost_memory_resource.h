#pragma once

#include <boost/container/pmr/memory_resource.hpp>
#include "memory_resource.h"

namespace eerie_leap::utilities::memory {

template <typename T>
class BoostExtMemoryResource : public boost::container::pmr::memory_resource {
private:
    ExtMemoryResource<T> pmr_;

public:
    void* do_allocate(size_t bytes, size_t align) override {
        return pmr_.do_allocate(bytes, align);
    }

    void do_deallocate(void* ptr, size_t bytes, size_t align) override {
        pmr_.do_deallocate(ptr, bytes, align);
    }

    bool do_is_equal(boost::container::pmr::memory_resource const& other) const noexcept override {
        return &other == this;
    }
};

static BoostExtMemoryResource<std::byte> ext_boost_byte_memory_resource;
static boost::json::storage_ptr ext_boost_json_storage_ptr(&ext_boost_byte_memory_resource);

} // namespace eerie_leap::utilities::memory
