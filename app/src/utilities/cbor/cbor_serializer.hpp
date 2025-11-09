#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <array>
#include <span>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_instance.h>
#include <zcbor_common.h>
#include <zcbor_encode.h>
#include <zcbor_decode.h>

#include <utilities/memory/heap_allocator.h>

namespace eerie_leap::utilities::cbor {

using namespace eerie_leap::utilities::memory;

// TODO: Figure out logging without LOG_MODULE_REGISTER and LOG_INSTANCE_PTR_DECLARE

template <typename T>
class CborSerializer {
public:
    using EncodeFn = int (*)(uint8_t*, size_t, const T*, size_t*);
    using DecodeFn = int (*)(const uint8_t*, size_t, T*, size_t*);
    using GetSerializingSizeFn = size_t (*)(const T&);

    CborSerializer(EncodeFn encoder, DecodeFn decoder, GetSerializingSizeFn getSerializingSizeFn)
        : encodeFn_(encoder), decodeFn_(decoder), getSerializingSizeFn_(getSerializingSizeFn) {}

    std::shared_ptr<ExtVector> Serialize(const T& obj, size_t *payload_len_out = nullptr) {
        auto buffer = make_shared_ext<ExtVector>(getSerializingSizeFn_(obj));

        size_t obj_size = 0;
        if(encodeFn_(buffer->data(), buffer->size(), &obj, &obj_size)) {
            // LOG_ERR("Failed to encode object.");
            return nullptr;
        }

        if (payload_len_out != nullptr)
            *payload_len_out = obj_size;

        auto buffer_serialized = make_shared_ext<ExtVector>(obj_size);
        std::copy(buffer->data(), buffer->data() + obj_size, buffer_serialized->begin());

        return buffer_serialized;
    }

    std::optional<T> Deserialize(std::span<const uint8_t> input) {
        T obj;
        if(decodeFn_(input.data(), input.size(), &obj, nullptr)) {
            // LOG_ERR("Failed to decode object.");
            return std::nullopt;
        }

        return obj;
    }

private:
    EncodeFn encodeFn_;
    DecodeFn decodeFn_;
    GetSerializingSizeFn getSerializingSizeFn_;
};

} // namespace eerie_leap::utilities::cbor
