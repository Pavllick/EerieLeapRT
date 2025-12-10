#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <boost/json.hpp>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_instance.h>
#include <zephyr/data/json.h>

#include "utilities/memory/memory_resource_manager.h"

namespace eerie_leap::configuration::json {

using namespace eerie_leap::utilities::memory;

template <typename T>
class JsonSerializer {
private:
    static std::pmr::string Encode(const T& config) {
        boost::json::value jv = boost::json::value_from(config, Mrm::GetBoostExtPmr());
        std::pmr::string result(Mrm::GetExtPmr());

        boost::json::serializer sr;
        sr.reset(&jv);

        char buffer[1024];

        while(!sr.done()) {
            boost::json::string_view sv = sr.read(buffer, sizeof(buffer));
            result.append(sv.data(), sv.size());
        }

        return result;
    }

    static T Decode(std::string_view json_str) {
        boost::json::value jv = boost::json::parse(json_str, Mrm::GetBoostExtPmr());

        return boost::json::value_to<T>(jv);
    }

public:
    JsonSerializer() = default;

    std::pmr::string Serialize(const T& obj, size_t *payload_len_out = nullptr) {
        LOG_MODULE_DECLARE(json_serializer_logger);

        auto json_str = Encode(obj);
        if(json_str.empty()) {
            LOG_ERR("Failed to serialize object.");
            return {};
        }

        return json_str;
    }

    pmr_unique_ptr<T> Deserialize(std::string_view json_str) {
        LOG_MODULE_DECLARE(json_serializer_logger);

        try {
            return make_unique_pmr<T>(Mrm::GetExtPmr(), Decode(json_str));
        } catch(...) {
            LOG_ERR("Failed to deserialize object.");
            return {};
        }
    }
};

} // namespace eerie_leap::configuration::json
