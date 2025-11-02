#include "subsys/mdf/serializable_block_base.h"

namespace eerie_leap::subsys::mdf {

SerializableBlockBase::SerializableBlockBase()
    : address_(0), is_serialized_(false) {}

uint64_t SerializableBlockBase::WriteToStream(std::streambuf& stream) {
    const auto block_data = Serialize();
    is_serialized_ = true;

    auto ret = stream.sputn(
        reinterpret_cast<const char*>(block_data.get()),
        static_cast<std::streamsize>(GetBlockSize()));

    if(ret != GetBlockSize())
        throw std::ios_base::failure("End of stream reached (EOF).");

    for(auto& child : GetChildren()) {
        if(child && !child->IsSerialized())
            ret += child->WriteToStream(stream);
    }

    return ret;
}

uint64_t SerializableBlockBase::GetAddress() const {
    return address_;
}

bool SerializableBlockBase::IsSerialized() const {
    return is_serialized_;
}

void SerializableBlockBase::Reset() {
    if(address_ == 0 && is_serialized_ == false)
        return;

    address_ = 0;
    is_serialized_ = false;

    for(auto& child : GetChildren()) {
        if(child)
            child->Reset();
    }
}

uint64_t SerializableBlockBase::ResolveAddress(uint64_t parent_address) {
    address_ = parent_address;
    uint64_t current_address = address_ + GetBlockSize();

    for(auto& child : GetChildren()) {
        if(child && child->GetAddress() == 0)
            current_address = child->ResolveAddress(current_address);
    }

    return current_address;
}

} // namespace eerie_leap::subsys::mdf
