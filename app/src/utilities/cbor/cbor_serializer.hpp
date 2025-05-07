#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include <array>
#include <span>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_instance.h>
#include <zcbor_common.h>
#include <zcbor_encode.h>
#include <zcbor_decode.h>

namespace eerie_leap::utilities::cbor {

// TODO: Figure out logging without LOG_MODULE_REGISTER and LOG_INSTANCE_PTR_DECLARE

template <typename T>
class CborSerializer {
public:
    using EncodeFn = int (*)(uint8_t*, size_t, const T*, size_t*);
    using DecodeFn = int (*)(const uint8_t*, size_t, T*, size_t*);

    CborSerializer(EncodeFn encoder, DecodeFn decoder)
        : encodeFn_(encoder), decodeFn_(decoder) {}

    size_t GetSerializingSize(const T& obj) {
        size_t obj_size = 0;

        static std::vector<uint8_t> buffer(sizeof(T));
        for(int i = 0; i < 100; i++) {
            if(!encodeFn_(buffer.data(), buffer.size(), &obj, &obj_size))
                return obj_size;

            buffer.resize(buffer.size() + 64);
        }

        return obj_size;
    }

    std::optional<std::span<const uint8_t>> Serialize(const T& obj, size_t *payload_len_out = nullptr) {
        static std::vector<uint8_t> buffer(GetSerializingSize(obj));

        size_t obj_size = 0;
        if(encodeFn_(buffer.data(), buffer.size(), &obj, &obj_size)) {
            // LOG_ERR("Failed to encode object!");
            return std::nullopt;
        }

        if (payload_len_out != nullptr)
            *payload_len_out = obj_size;

        return std::span<const uint8_t>(buffer.data(), obj_size);
    }

    std::optional<T> Deserialize(std::span<const uint8_t> input) {
        T obj;
        if(decodeFn_(input.data(), input.size(), &obj, nullptr)) {
            // LOG_ERR("Failed to decode object!");
            return std::nullopt;
        }

        return obj;
    }

private:
    EncodeFn encodeFn_;
    DecodeFn decodeFn_;
};

} // namespace eerie_leap::utilities::cbor
