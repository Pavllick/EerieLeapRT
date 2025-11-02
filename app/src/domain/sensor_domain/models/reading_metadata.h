#pragma once

#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

#include "subsys/canbus/can_frame.h"

#include "reading_metadata_tag.h"

namespace eerie_leap::domain::sensor_domain::models {

using namespace eerie_leap::subsys::canbus;

using ReadingMetadataValue = std::variant<
    std::monostate,
    int,
    float,
    std::string,
    bool,
    CanFrame
>;

struct ReadingMetadata {
    std::unordered_map<ReadingMetadataTag, ReadingMetadataValue> tags;

    void AddTag(const ReadingMetadataTag tag, const ReadingMetadataValue& value) {
        tags[tag] = value;
    }

    template <typename T>
    void AddTag(const ReadingMetadataTag tag, const T& value) {
        tags[tag] = value;
    }

    template <typename T>
    std::optional<T> GetTag(const ReadingMetadataTag tag) const {
        if(tags.find(tag) == tags.end())
            return std::nullopt;

        if(!std::holds_alternative<T>(tags.at(tag)))
            return std::nullopt;

        return std::get<T>(tags.at(tag));
    }
};

} // namespace eerie_leap::domain::sensor_domain::models
