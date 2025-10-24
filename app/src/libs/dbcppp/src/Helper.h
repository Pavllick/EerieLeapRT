#pragma once

#include <cstdint>

#include <zephyr/sys/byteorder.h>

#include "Export.h"
#include "EndianConfig.h"

namespace dbcppp
{
    class NodeImpl;
    struct DBCPPP_API SharedNodeCmp
    {
        bool operator()(const NodeImpl& lhs, const NodeImpl& rhs) const;
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
