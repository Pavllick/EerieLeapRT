#pragma once

#include <unordered_map>
#include <string>

#include "reading_metadata_tag.h"

namespace eerie_leap::domain::sensor_domain::models {

struct ReadingMetadata {
    std::unordered_map<ReadingMetadataTag, std::string> tags;

    void AddTag(ReadingMetadataTag key, const std::string& value) {
        tags[key] = value;
    }
};

} // namespace eerie_leap::domain::sensor_domain::models
