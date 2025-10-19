#pragma once

#include <cstdint>
#include <cstring>
#include <memory>
#include <array>
#include <utility>
#include <concepts>
#include <type_traits>

#include "subsys/mdf/i_block.h"
#include "i_block_links.h"

namespace eerie_leap::subsys::mdf::utilities {

template<typename T>
concept IntEnum =
    std::is_enum_v<T> &&
    std::is_same_v<std::underlying_type_t<T>, int>;

template<IntEnum LinkType, int N>
class BlockLinks : public IBlockLinks {
private:
    std::array<std::shared_ptr<IBlock>, N> links_;
    static const int LINK_SIZE_BYTES = 8;

public:
    BlockLinks() = default;

    int Count() const override {
        return N;
    }

    void SetLink(LinkType type, std::shared_ptr<IBlock> link) {
        links_[std::to_underlying(type)] = link;
    }

    uint64_t GetLinksSizeBytes() const override {
        return N * LINK_SIZE_BYTES;
    }

    std::shared_ptr<IBlock> GetLink(LinkType type) const {
        return links_[std::to_underlying(type)];
    }

    std::unique_ptr<uint8_t[]> Serialize() const override {
        const uint64_t size = GetLinksSizeBytes();
        auto buffer = std::make_unique<uint8_t[]>(size);
        std::memset(buffer.get(), 0, size);

        uint64_t offset = 0;

        for(auto& link : links_) {
            if(link) {
                auto address = link->GetAddress();
                if(address == 0)
                    throw std::runtime_error("Invalid Block address");

                std::memcpy(buffer.get() + offset, &address, LINK_SIZE_BYTES);
            }

            offset += LINK_SIZE_BYTES;
        }

        return buffer;
    }
};

} // namespace eerie_leap::subsys::mdf::utilities
