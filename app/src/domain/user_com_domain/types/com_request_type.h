#pragma once

#include <cstdint>
#include <cstddef>

namespace eerie_leap::domain::user_com_domain::types {

enum class ComRequestType : uint16_t {
    GET_RESOLVE_SERVER_ID_GUID = 1,
    GET_STATUS = 2,

    SET_RESOLVE_SERVER_ID = 127,
    SET_RESOLVE_SERVER_ID_GUID = 128,
    SET_READING = 129,
    SET_ACK = 130,
    SET_STATUS_UPDATE_OK = 131,
    SET_STATUS_UPDATE_FAIL = 132,
};

struct ComRequestSize {
    static constexpr size_t GetSize(ComRequestType com_request_type) {
        switch(com_request_type) {
            case ComRequestType::GET_RESOLVE_SERVER_ID_GUID:
                return 8;
            case ComRequestType::GET_STATUS:
                return 2;
            default:
                return 0;
        }
    }
};

} // namespace eerie_leap::domain::user_com_domain::types
