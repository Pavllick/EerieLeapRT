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
private:
    const json_obj_descr* object_descriptor_;
    size_t object_descriptor_size_;

public:
    JsonSerializer(const json_obj_descr* object_descriptor, size_t object_descriptor_size)
        : object_descriptor_(object_descriptor), object_descriptor_size_(object_descriptor_size) {}

    ext_unique_ptr<ExtVector> Serialize(const T& obj, size_t *payload_len_out = nullptr) {
        LOG_MODULE_DECLARE(json_serializer_logger);

        auto buf_size = json_calc_encoded_len(object_descriptor_, object_descriptor_size_, &obj);
        if(buf_size < 0) {
            LOG_ERR("Failed to calculate buffer size.");
            return nullptr;
        }

        // Add 1 for null terminator
        auto buffer = make_unique_ext<ExtVector>(buf_size + 1);

        int res = json_obj_encode_buf(object_descriptor_, object_descriptor_size_, &obj, (char*)buffer->data(), buffer->size());
        if(res != 0) {
            LOG_ERR("Failed to serialize object.");
            return nullptr;
        }

        return buffer;
    }

    ext_unique_ptr<T> Deserialize(std::span<const uint8_t> input) {
        LOG_MODULE_DECLARE(json_serializer_logger);

        auto obj = make_unique_ext<T>();
        const int expected_return_code = BIT_MASK(object_descriptor_size_);

        int ret = json_obj_parse((char*)input.data(), input.size(), object_descriptor_, object_descriptor_size_, obj.get());
        if(ret != expected_return_code) {
            LOG_ERR("Failed to parse JSON object.");
            return nullptr;
        }

        return obj;
    }
};

} // namespace eerie_leap::configuration::json
