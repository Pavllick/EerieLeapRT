#pragma once

#include <cstdint>
#include <memory>

#include "block_links.h"

namespace eerie_leap::subsys::mdf::utilities {

enum class LinkTypeEmpty { };

class BlockLinksEmpty : public BlockLinks<LinkTypeEmpty, 0> {
public:
    BlockLinksEmpty() = default;

    int Count() const override {
        return 0;
    }

    uint64_t GetLinksSizeBytes() const override {
        return 0;
    }

    const std::vector<std::shared_ptr<ISerializableBlock>> GetLinks() const override {
        return {};
    }

    std::unique_ptr<uint8_t[]> Serialize() const override {
        return {};
    }
};

} // namespace eerie_leap::subsys::mdf::utilities
