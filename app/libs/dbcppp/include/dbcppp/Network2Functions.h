
#pragma once

#include <boost/format.hpp>
#include <ostream>
#include <tuple>
#include <ostream>

#include "Network.h"

namespace dbcppp
{
    namespace Network2DBC
    {
        using na_t = std::tuple<const INetwork&, const IAttribute&>;
        std::ostream& operator<<(std::ostream& os, const na_t& na);
        std::ostream& operator<<(std::ostream& os, const IAttributeDefinition& ad);
        std::ostream& operator<<(std::ostream& os, const IBitTiming& bt);
        std::ostream& operator<<(std::ostream& os, const IEnvironmentVariable& ev);
        std::ostream& operator<<(std::ostream& os, const IMessage& m);
        std::ostream& operator<<(std::ostream& os, const INetwork& net);
        std::ostream& operator<<(std::ostream& os, const INode& n);
        std::ostream& operator<<(std::ostream& os, const ISignal& s);
        std::ostream& operator<<(std::ostream& os, const ISignalType& st);
        std::ostream& operator<<(std::ostream& os, const IValueTable& vt);
    }
    namespace Network2Human
    {
        std::ostream& operator<<(std::ostream& os, const INetwork& net);
        std::ostream& operator<<(std::ostream& os, const IMessage& msg);
    }
}
