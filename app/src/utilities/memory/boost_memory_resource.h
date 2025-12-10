#pragma once

#include <boost/container/pmr/memory_resource.hpp>

#include "memory_resource.h"

namespace eerie_leap::utilities::memory {

class BoostExtMemoryResource : public boost::container::pmr::memory_resource {
private:
    std::pmr::memory_resource* pmr_;

public:
    BoostExtMemoryResource(std::pmr::memory_resource* mr) : pmr_(mr) {}

    void* do_allocate(size_t bytes, size_t align) override {
        return pmr_->allocate(bytes, align);
    }

    void do_deallocate(void* ptr, size_t bytes, size_t align) override {
        pmr_->deallocate(ptr, bytes, align);
    }

    bool do_is_equal(boost::container::pmr::memory_resource const& other) const noexcept override {
        return &other == this;
    }
};

} // namespace eerie_leap::utilities::memory
