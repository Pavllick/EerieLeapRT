#include "subsys/mdf/serializable_block_base.h"

namespace eerie_leap::subsys::mdf {

SerializableBlockBase::SerializableBlockBase(): address_(0) {}

uint64_t SerializableBlockBase::WriteToStream(std::streambuf& stream) const {
    const auto block_data = Serialize();

    auto ret = stream.sputn(
        reinterpret_cast<const char*>(block_data.get()),
        static_cast<std::streamsize>(GetSize()));

    if(ret != GetSize())
        throw std::ios_base::failure("End of stream reached (EOF).");

    for(auto& child : GetChildren()) {
        if(child)
            ret += child->WriteToStream(stream);
    }

    return ret;
}

uint64_t SerializableBlockBase::GetAddress() const {
    return address_;
}

uint64_t SerializableBlockBase::ResolveAddress(uint64_t parent_address) {
    address_ = parent_address;
    uint64_t current_address = address_ + GetSize();

    for(auto& child : GetChildren()) {
        if(child)
            current_address = child->ResolveAddress(current_address);
    }

    return current_address;
}

} // namespace eerie_leap::subsys::mdf
