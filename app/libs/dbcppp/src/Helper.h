#pragma once

#include <cstdint>

#include <zephyr/sys/byteorder.h>

#include "EndianConfig.h"

namespace dbcppp
{
    class Node;
    struct SharedNodeCmp
    {
        bool operator()(const Node& lhs, const Node& rhs) const;
    };
    inline void native_to_big_inplace(uint64_t& value)
    {
        if constexpr (dbcppp::Endian::Native == dbcppp::Endian::Little)
        {
            BSWAP_64(value);
        }
    }
    inline void native_to_little_inplace(uint64_t& value)
    {
        if constexpr (dbcppp::Endian::Native == dbcppp::Endian::Big)
        {
            BSWAP_64(value);
        }
    }
}
