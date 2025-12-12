#pragma once

#include <eerie_memory.hpp>

#include "memory_resource.h"
#include "boost_memory_resource.h"

namespace eerie_leap::utilities::memory {

using namespace eerie_memory;

class Mrm {
private:
    static ExtMemoryResource ext_memory_resource_;
    static BoostExtMemoryResource ext_boost_memory_resource;

public:
    static std::pmr::memory_resource* GetDefaultPmr();
    static std::pmr::memory_resource* GetExtPmr();
    static boost::container::pmr::memory_resource* GetBoostExtPmr();
};

} // namespace eerie_leap::utilities::memory
