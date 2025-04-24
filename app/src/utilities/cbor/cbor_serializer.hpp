#pragma once

#include <optional>
#include <array>
#include <zephyr/kernel.h>
#include <zcbor_common.h>
#include <zcbor_encode.h>
#include <zcbor_decode.h>

namespace eerie_leap::utilities::cbor {

LOG_MODULE_REGISTER(cbor_serializer_logger);

template <typename T, size_t MaxSize = 256>
class CborSerializer {
public:
    using Buffer = std::array<uint8_t, MaxSize>;

    using EncodeFn = int (*)(uint8_t*, size_t, const T*, size_t*);
    using DecodeFn = int (*)(const uint8_t*, size_t, T*, size_t*);

    CborSerializer(EncodeFn encoder, DecodeFn decoder) : encodeFn_(encoder), decodeFn_(decoder) {}

    std::optional<std::span<const uint8_t>> Serialize(const T& obj, size_t *payload_len_out = nullptr) {
        static Buffer buffer{};

        size_t obj_size = 0;
        if(encodeFn_(buffer.data(), buffer.size(), &obj, &obj_size)) {
            LOG_ERR("Failed to encode object!");
            return std::nullopt;
        }

        if (payload_len_out != nullptr)
            *payload_len_out = obj_size;

        return std::span<const uint8_t>(buffer.data(), obj_size);
    }

    std::optional<T> Deserialize(std::span<const uint8_t> input) {
        T obj;
        if(decodeFn_(input.data(), input.size(), &obj, nullptr)) {
            LOG_ERR("Failed to decode object!");
            return std::nullopt;
        }

        return obj;
    }

private:
    EncodeFn encodeFn_;
    DecodeFn decodeFn_;
};

} // namespace eerie_leap::utilities::cbor