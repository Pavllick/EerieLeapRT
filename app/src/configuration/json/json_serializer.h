#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_instance.h>
#include <zephyr/data/json.h>

#include "utilities/memory/heap_allocator.h"

namespace eerie_leap::configuration::json {

using namespace eerie_leap::utilities::memory;

template <typename T>
class JsonSerializer {
public:
    using EncodeFn = std::string (*)(const T&);
    using DecodeFn = T (*)(std::string_view);

    JsonSerializer(EncodeFn encoder, DecodeFn decoder)
        : encodeFn_(encoder), decodeFn_(decoder) {}

    ext_unique_ptr<std::string> Serialize(const T& obj, size_t *payload_len_out = nullptr) {
        LOG_MODULE_DECLARE(json_serializer_logger);

        auto json_str = encodeFn_(obj);
        if(json_str.empty()) {
            LOG_ERR("Failed to serialize object.");
            return nullptr;
        }

        return make_unique_ext<std::string>(json_str);
    }

    ext_unique_ptr<T> Deserialize(std::string_view json_str) {
        LOG_MODULE_DECLARE(json_serializer_logger);

        try {
            return make_unique_ext<T>(decodeFn_(json_str));
        } catch(...) {
            LOG_ERR("Failed to deserialize object.");
            return nullptr;
        }
    }

private:
    EncodeFn encodeFn_;
    DecodeFn decodeFn_;
};

} // namespace eerie_leap::configuration::json
