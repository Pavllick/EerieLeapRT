#include "memory_resource_manager.h"

namespace eerie_leap::utilities::memory {

ExtMemoryResource Mrm::ext_memory_resource_;
BoostExtMemoryResource Mrm::ext_boost_memory_resource(&ext_memory_resource_);

std::pmr::memory_resource* Mrm::GetDefaultPmr() {
    return std::pmr::get_default_resource();
}

std::pmr::memory_resource* Mrm::GetExtPmr() {
    return &ext_memory_resource_;
}

boost::container::pmr::memory_resource* Mrm::GetBoostExtPmr() {
    return &ext_boost_memory_resource;
}

} // namespace eerie_leap::utilities::memory
